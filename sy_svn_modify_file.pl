#!/usr/bin/perl

use File::Copy;
use File::Find;
use File::Spec;
use File::Path;
use File::Basename;
use Cwd;

my $ARGC = @ARGV;
my $path_separator = '/';

if($ARGC != 2)
{
    warn "parameter is error!!!, get_svn_modify_file.pl src_path save_path\r\n";
    return 1;
}

my $path = $ARGV[0];
my $patch_path = $ARGV[1];
my $tmp_file = $path.$path_separator."pick_svn_modify.txt";
local ($current_work_root) = getcwd;
my $make_item;
my $src_item;
my $is_add;

$patch_path =~ s/\\/\//g;
$patch_path =~ s/^\s+//;
$patch_path =~ s/\s+$//;

if(!($patch_path =~ m/\/$/))
{
    $patch_path = $patch_path.$path_separator;
}

my $patch_src = $patch_path . "old/";

$patch_path = $patch_path . "new/";

$current_work_root = $current_work_root.$path_separator;

system("svn status $path > $tmp_file");

open (inf,$tmp_file) || die ("$tmp_file!!!");
my @file_all = <inf>;
close (inf);  

foreach $item (@file_all)
{
    if($item =~ m/^\M/ || $item =~ m/^A/)
    {
        if ($item =~ m/^A/)
        {
            $is_add = 1;
        }
        else
        {
            $is_add = 0;
        }
        
        $item = substr($item,7);
        $item =~ s/\\/\//g;
        $item =~ s/^\s+//;
        $item =~ s/\s+$//;
        
        $item =~ s/\Q$current_work_root\E//;
        
        $make_item = $patch_path.$item;
        $make_item =~ s/\\/\//g;
        
        $src_item = $patch_src.$item;
        $src_item =~ s/\\/\//g;
        
        if(-d $item)
        {
            if(!(-d $make_item))
            {
                RecursiveMkdir($make_item);
            }
            
            if(!(-d $src_item))
            {
                if ($is_add == 0)
                {
                    RecursiveMkdir($src_item);
                }
            }
        }
        elsif(-e $item)
        {
            my $dir_name = dirname($make_item);
            if(!(-d $dir_name))
            {
                RecursiveMkdir($dir_name);
            }
            copy($item,$make_item);


            if ($is_add == 0)
            {
                system("svn revert $item");
                $dir_name = dirname($src_item);
                if(!(-d $dir_name))
                {
                    RecursiveMkdir($dir_name);
                }
                copy($item,$src_item);

                copy($make_item, $item);
            }
        }
    }
}

if(-e $tmp_file)
{
    unlink($tmp_file);
}

sub RecursiveMkdir()
{
    if (!(-d $_[0]))
    {
        RecursiveMkdir(dirname($_[0]));
        mkdir($_[0]);
    }
}

