#!/usr/bin/perl

use File::Copy;
use File::Path;

$base = "DreamChess.app/Contents";

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
    system("install_name_tool -change $dep \@executable_path/../Frameworks/$deps{$dep} $filename");
  }
}

sub copylibs {
  foreach my $dep (keys(%deps)) {
    my $filename = "DreamChess.app/Contents/Frameworks/$deps{$dep}";
    print "Importing $dep\n";
    copy($dep, $filename);
    system("install_name_tool -id \@executable_path/../Frameworks/$deps{$dep} $filename");
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

open(file, "include/svn_version.h");
my $version = join("", <file>);
close(file);
$version =~ s/^.*("|:)([0-9M]*)"(.|\n)*$/$2/;
open(FH, "macosx/Info.plist");
my $plist = join("", <FH>);
close(FH);
$plist =~ s/SVNVERSION/$version/;
open(FH, ">$base/Info.plist");
print FH $plist;
close(FH);

system("cp -R ../data/* $base/Resources/");
system("find $base/Resources/ -type d -name '.svn' -print | xargs rm -rf");
system("cp dreamer/dreamer $base/MacOS/");
