#!/usr/bin/perl
#
# @author       iota square [i2]
# <pre>
# ██╗ ██████╗ ████████╗ █████╗ ██████╗
# ██║██╔═══██╗╚══██╔══╝██╔══██╗╚════██╗
# ██║██║   ██║   ██║   ███████║ █████╔╝
# ██║██║   ██║   ██║   ██╔══██║██╔═══╝
# ██║╚██████╔╝   ██║   ██║  ██║███████╗
# ╚═╝ ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚══════╝
# </pre>
#
# @file         map_parser.pl
# @date         12-10-2019
# @brief       	Utility to parse map file into different segments.
#
# @copyright    GNU GPU v3
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Free Software, Hell Yeah!
#

$printall = 0;
$warn = 0;
$all_ram = 0;

# check for input aguments
if ( ! $ARGV[0] )
{
  print "Usage map_parser.pl [-a] [-w] [-r] <map file filename>";
  print "           -a = show all sections";
  print "           -w = show more warnings";
  print "           -r = all modules in RAM";
  exit;
}

# command line parameters
foreach ( @ARGV )
{
  if ( $_ eq "-a" )
  {
    $printall = 1;
  }
  elsif ( $_ eq "-w" )
  {
    $warn = 1;
  }
  elsif ( $_ eq "-r" )
  {
    $all_ram = 1;
  }
  else
  {
    $filename = $_;
  }
}

#open the file
open INFILE, $filename or die "Failed to open " . $filename;
@file = <INFILE>;
close INFILE;

$file_cont = join('',@file);

#Get the start of FLASH and ram

$file_cont =~ m/.*Memory Configuration.*Attributes(.*)Linker script and memory map.*/sgi;

$mem_config = $1;

#my $all_ram;
#$all_ram = 1;

#Capture all flash sections (look for xr attribute, without w, to signify flash)
while( $mem_config =~ m/(ROM|FLASH|APP_CODE)\s+0x(\S*)\s+0x(\S*)\s/sgi )
{
  push(@flash_section_name,$1);
  push(@start_flash_section, hex($2));
  push(@length_flash_section, hex($3));
#  $all_ram = 0;
}

#Capture all ram sections (look for xrw attribute to signify ram)
while( $mem_config =~ m/(FLOPS|RAM|DDR)\s+0x(\S*)\s+0x(\S*)\s+xrw/sgi )
{
  push(@ram_section_name,$1);
  push(@start_ram, hex($2));
  push(@length_ram, hex($3));
}

#remove stuff above the memory map
$file_cont =~ s/.*Linker script and memory map(.*)/$1/sgi;

#remove stuff below the memory map
$file_cont =~ s/(.*)Cross Reference Table.*/$1/sgi;

%module_totals = ( );

$total_flash = 0;
$total_ram = 0;
$total_other = 0;

$max_flash = 0;
$max_ram = 0;
$max_other = 0;

if ( $printall == 1 )
{
  print "Memory Area, Module, Name, Size, Address, Decimal Address, Filename, See bottom for collated totals\n";
}

$ram_known_diff = 0;
$flash_known_diff = 0;
$other_known_diff = 0;

while ( $file_cont =~ m/\n [\.\*]?(\S+?)\*?\s+0x(\S+)\s+0x(\S+)/gi )
{
  $section = $1;
  $hexaddress = $2;
  $decaddress = hex($2);
  $size = hex($3);

  if (!( $section eq "fill" ))
  {
    $file_cont =~ m/\s+(\S+)/gi;
    $modulefn = $1;
  }

  if(($section ne "debug_info" ) &&
    ( $section ne "debug_macinfo" ) &&
    ( $section ne "debug_str" ) &&
    ( $section ne "debug_line" ) &&
    ( $section ne "debug_loc" ) &&
    ( $section ne "debug_frame" ) &&
    ( $section ne "debug_abbrev" ) &&
    ( $section ne "debug_pubnames" ) &&
    ( $section ne "debug_aranges" ) &&
    ( $section ne "ARM.attributes" ) &&
    ( $section ne "comment" ) &&
    ( $section ne "debug_ranges" ) &&
    ( $section ne "debug_pubtypes" ) &&
    ( $size != 0 ) &&
    ( $decaddress != 0 ) )
  {
    if( $all_ram == 1 )
    {
      $area = "RAM";
      $total_ram += $size;
    }
    else
    {
      my $loop_index=0;
      foreach $flash_section_name (@flash_section_name)
      {
		    $start_flash_section =  @start_flash_section[$loop_index];
		    $length_flash_section = @length_flash_section[$loop_index];
		    $end_flash_section = $start_flash_section + $length_flash_section;
		    if( ( $decaddress >= $start_flash_section) && ( $decaddress < $end_flash_section) )
		    {
          $area = "FLASH";
          $total_flash += $size;
		    }
		    $loop_index++;
      }
      my $loop_index=0;
      foreach $ram_section_name (@ram_section_name)
      {
        $start_ram = @start_ram[$loop_index];
        $length_ram = @length_ram[$loop_index];
        $end_ram = $start_ram + $length_ram;
        if( ( $decaddress >= $start_ram) && ( $decaddress < $end_ram) )
        {
          $area = "RAM";
          $total_ram += $size;
        }
        $loop_index++;
      }
    }
    $module = process_section( $area, $section, $size, $modulefn, $decaddress, $hexaddress );
#    if ($printall == 1 )
#    {
#      print_sections( );
#    }
  }
}

# Process sections that are in both flash and ram due to having inital values load from flash into ram variables
while ( $file_cont =~ m/\n.(\S+)\s+0x(\S+)\s+0x(\S+) load address 0x(\S+)/gi )
{
  $area = "FLASH";
  $section = "RAM Initialisation - $1";
  $size = hex($3);
  $hexaddress = $4;
  $decaddress = hex($4);
  $modulefn = "";

  if ( $decaddress < $start_flash + $length_flash)
  {
    $total_flash += $size;

    if ( $max_flash < $decaddress + $size )
    {
      $max_flash = $decaddress + $size;
    }

    if ( $warn && ( ( $total_flash != ($max_flash - $start_flash) ) && ( $flash_known_diff != (($max_flash - $start_flash) - $total_flash)) ) )
    {
      $flash_known_diff = ($max_flash - $start_flash) - $total_flash;
      print "WARNING: FLASH Max mismatch @ 0x$hexaddress. Max Flash = " . ($max_flash - $start_flash) . ". Total Flash = $total_flash. \n";
  	}
  }

  if ( $size != 0 )
  {
    $module = process_section( $area, $section, $size, $modulefn, $decaddress, $hexaddress );
  }
  if ($printall == 1 )
  {
    print "\n";
    print_sections( );
    print "\n";
  }
}


if ( $printall == 1 )
{
  print "\n";
}

# Get target name from map filename

$filename       =~ m/^(.*)\.map/;
my $target_name = $filename;
$target_name    =~ s/^(.*)\.map/$1/i;
$target_name    =~ s/.*\/(.*)$/$1/i;
print "$target_name\n";

print_module_total( );

sub print_sections
{
  print "Memory Area, Section Name, Address (Hex), Address (decimal), Size (bytes), Module filename\n";
  foreach $module (sort keys %module_totals)
  {
    print "$module, $module_totals{$module}\n";
  }
  print "$area, $section , 0x$hexaddress , $decaddress, $size, $module, $modulefn\n";
}


sub process_section( $area, $section, $size, $modulefn, $decaddress, $hexaddress )
{
  $module = "Other";

  if ( $modulefn =~ m/\/lib_stm32f4xx_hal\.a/sgi )
  {
    $module = "STM32F4xx HAL";
  }
  elsif ( $modulefn =~ m/\/lib_freertos_v10_2_1\.a/sgi )
  {
    $module = "FreeRTOS 10.2.1";
  }
  elsif ( $modulefn =~ m/\/i2_assert\.o/sgi ||
          $modulefn =~ m/\/i2_fifo\.o/sgi   ||
          $modulefn =~ m/\/i2_led\.o/sgi )
  {
    $module = "iota2 Interface Driver";
  }
  elsif ( $modulefn =~ m/\/i2_stm32f4xx_hal_gpio\.o/sgi   ||
          $modulefn =~ m/\/i2_stm32f4xx_hal_uart\.o/sgi   ||
          $modulefn =~ m/\/i2_stm32f4xx_hal_uart\.o/sgi   ||
          $modulefn =~ m/\/i2_stm32f4xx_hal_clock\.o/sgi  ||
          $modulefn =~ m/\/i2_stm32f4xx_hal_spi\.o/sgi    ||
          $modulefn =~ m/\/i2_stm32f4xx_hal_common\.o/sgi ||
          $modulefn =~ m/\/i2_stm32f4xx_hal_rtc\.o/sgi )
  {
    $module = "iota2 STM32F4xx HAL Driver";
  }
  elsif ( $modulefn =~ m/\/system_stm32f4xx\.o/sgi    ||
          $modulefn =~ m/\/startup_stm32f407xx\.o/sgi ||
          $modulefn =~ m/\/stm32f4xx_it\.o/sgi )
  {
    $module = "STM32F4xx System";
  }
  elsif ( $modulefn =~ m/\/app\.\w+\.o/sgi )
  {
    $module = "Application";
  }
  elsif ( $modulefn =~ m/arm-none-eabi/sgi ||
          $modulefn =~ m/[\\\/]libc\.a/sgi ||
          $modulefn =~ m/[\\\/]libgcc\.a/sgi ||
          $modulefn =~ m/[\\\/]libm\.a/sgi ||
          $modulefn =~ m/[\\\/]common_GCC\.a/sgi ||
          $modulefn =~ m/\/common\/GCC\/\S+\.o/sgi ||
          $modulefn =~ m/\/platform\/GCC\/\S+\.o/sgi )
  {
    $module = "libc";
  }

  $module_totals{"$module"}{"$area"} += $size;

  return $module;
}

sub print_module_total( )
{
  if ( $all_ram != 1 )
  {
    if ( $printall == 1 )
    {
      print ":------------------------------------:---------:---------:\n";
      print ":                                    :         :  Static :\n";
      print ": Module                             : Flash   :   RAM   :\n";
      print ":------------------------------------:---------:---------:\n";
      foreach $module (sort {"\L$a" cmp "\L$b"} keys %module_totals)
      {
        print sprintf(": %-34.34s : %7d : %7d :\n", $module, $module_totals{$module}{'FLASH'},  $module_totals{$module}{'RAM'});
      }
      print         ":------------------------------------:---------:---------:\n";
      print sprintf(": TOTAL (bytes)                      : %7d : %7d :\n", $total_flash,  $total_ram);
      print         ":------------------------------------:---------:---------:\n";
    }
    else
    {
      print "+------------------------------------+---------+---------+\n";
      print "|                                    |         |  Static |\n";
      print "|              Module                |  Flash  |   RAM   |\n";
      print "+------------------------------------+---------+---------+\n";
      foreach $module (sort {"\L$a" cmp "\L$b"} keys %module_totals)
      {
	      print sprintf("| %-34.34s | %7d | %7d |\n", $module, $module_totals{$module}{'FLASH'},  $module_totals{$module}{'RAM'});
      }
      print         "+------------------------------------+---------+---------+\n";
      print sprintf("| TOTAL (bytes)                      | %7d | %7d |\n", $total_flash,  $total_ram);
      print         "+------------------------------------+---------+---------+\n";
    }
  }
  else
  {
    if ( $printall == 1 )
    {
      print ":------------------------------------:---------:\n";
      print ":                                    :  Static :\n";
      print ": Module                             :   RAM   :\n";
      print ":------------------------------------:---------:\n";
      foreach $module (sort {"\L$a" cmp "\L$b"} keys %module_totals)
      {
        print sprintf(": %-34.34s : %7d :\n", $module, $module_totals{$module}{'RAM'});
      }
      print         ":------------------------------------:---------:\n";
      print sprintf(": TOTAL (bytes)                      : %7d :\n", $total_ram);
      print         ":------------------------------------:---------:\n";
    }
    else
    {
      print "+------------------------------------+---------+\n";
      print "|                                    |  Static |\n";
      print "|              Module                |   RAM   |\n";
      print "+------------------------------------+---------+\n";
      foreach $module (sort {"\L$a" cmp "\L$b"} keys %module_totals)
      {
        print sprintf("| %-34.34s | %7d |\n", $module,  $module_totals{$module}{'RAM'});
      }
      print         "+------------------------------------+---------+\n";
      print sprintf("| TOTAL (bytes)                      | %7d |\n", $total_ram);
      print         "+------------------------------------+---------+\n";
  	}
  }
  if ( $total_other != 0)
  {
    print "WARNING: $total_other bytes unaccounted for (neither FLASH or RAM)\n";
  }
  print "\n"
}

# *********************** (C) COPYRIGHT iota2 ***[i2]******END OF FILE**********
