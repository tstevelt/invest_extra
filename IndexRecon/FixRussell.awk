#!/bin/awk -f
BEGIN \
{	FS = " " ; OFS = ","
}
{
#	print $0
	printf ( "%s,", $NF )
	for ( fld = 1; fld < NF; fld++ )
	{
		printf ( "%s ", $fld )
	}
	printf ( "\n" )
}
END \
{
}
