#!/usr/bin/perl

use File::Copy;
use File::Path;

$base = "DreamChess.app/Contents";

if ($#ARGV != 0) {
  print "Invalid arguments\n";
  exit;
}
$top_srcdir = $ARGV[0];
print "Building DreamChess.app\n";

sub finddeps {
  my($filename) = @_;
  my @libs = `otool -L $filename`;

  foreach (@libs) {
    chomp;
    if (/:$/) {
      next;
    }
    if (/^\t(\/System)|(\/usr\/lib)|($filename)/) {
      next;
    }
    (my $orglib = $_) =~ s/^\t([^ ]*) .*$/$1/;
    if (!exists($deps{$orglib})) {
      (my $libfile = $orglib) =~ s/[^ ]*\/([^\/ ]*)$/$1/;
      $deps{$orglib} = $libfile;
      finddeps($orglib);
    }
  }
}

sub fixdeps {
  my($filename) = @_;

  foreach my $dep (keys(%deps)) {
    system("install_name_tool -change \"$dep\" \"\@executable_path/../Frameworks/$deps{$dep}\" \"$filename\"");
  }
}

sub copylibs {
  foreach my $dep (keys(%deps)) {
    my $filename = "DreamChess.app/Contents/Frameworks/$deps{$dep}";
    print "Importing $dep\n";
    copy($dep, $filename);
    system("install_name_tool -id \"\@executable_path/../Frameworks/$deps{$dep}\" \"$filename\"");
    fixdeps($filename);
  }
}

rmtree("DreamChess.app");
mkpath("$base/MacOS");
mkpath("$base/Resources");
mkpath("$base/Frameworks");
open(pkginfo, ">>$base/PkgInfo");
print pkginfo "APPL????\n";
close pkginfo;

finddeps("dreamchess");

$filename = "DreamChess.app/Contents/MacOS/DreamChess";
copy("dreamchess", $filename);
$mode = 0755;
chmod($mode, $filename);
fixdeps($filename);
copylibs;

open(file, "include/git_rev.h") or open(file, "$top_srcdir/src/include/git_rev.h") or die $!;
my $version = join("", <file>);
close(file);
$version =~ s/^.*("|:)([0-9a-f]*)"(.|\n)*$/$2/;
open(FH, "macosx/Info.plist") or die $!;
my $plist = join("", <FH>);
close(FH);
$plist =~ s/GITREV/$version/;
open(FH, ">$base/Info.plist") or die $!;
print FH $plist;
close(FH);

system("cp -R \"$top_srcdir/data/\"* \"$base/Resources/\"");
system("cp dreamer/dreamer \"$base/MacOS/\"");
system("find DreamChess.app -exec chmod u+w '{}' ';'");
#system("cp -R ../../../music/trunk/data/music $base/Resources");
