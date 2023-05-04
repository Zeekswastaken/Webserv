#!/usr/bin/perl

use strict;
use CGI ':standard';

my $name = param('name');
my $gender = param('gender');
my $profession = param('profession');
my @sports = param('sport');

my $list;

if (@sports)
{
    $list = join ', ', @sports;
}
else
{
    $list = 'Null';
}

print header,
start_html(-title=>$name),
h1("Hello, $name"),
h3 p('You have Submitted the following Data:'),
h4 table(Tr(td('Name:'),
h4 td($name)),
h4 Tr(td('Gender:'),
h4 td($gender)),
h4 Tr(td('Profession:'),
h4 td($profession)),
h4 Tr(td('Sports:'),
h4 td($list))),
end_html;