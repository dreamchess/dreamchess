#!/usr/bin/perl

use File::Copy;
use File::Path;

$base = "DreamChess.app/Contents";

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
    (my $orglib = $_) =~ s/^\t([^ ]*) .*$/\1/;
    if (!exists($deps{$orglib})) {
      (my $libfile = $orglib) =~ s/[^ ]*\/([^\/ ]*)$/\1/;
      $deps{$orglib} = $libfile;
      finddeps($orglib);
    }
  }
}

sub fixdeps {
  my($filename) = @_;

  print "Fixing dependencies for $filename:\n";
  foreach my $dep (keys(%deps)) {
    print "\t$dep\n";
    system("install_name_tool -change $dep \@executable_path/../Frameworks/$deps{$dep} $filename");
  }
}

sub copylibs {
  foreach my $dep (keys(%deps)) {
    my $filename = "DreamChess.app/Contents/Frameworks/$deps{$dep}";
    print "Copying $dep to $filename\n";
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
