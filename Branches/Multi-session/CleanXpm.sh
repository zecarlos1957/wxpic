#!/bin/bash
#------------------------------------------------------
#-- Name: CleanXpm.sh
#-- Purpose: Make Xpm compatible with gcc 4.4
#-- Author: Philippe Chevrier
#------------------------------------------------------
for XPM in resources/*.xpm; do
	grep "static char *" $XPM
	if [ $? = 0 ]; then
	{
		sed -e 's/static char/static const char/' $XPM > ${XPM}.tmp
		rm $XPM
		mv ${XPM}.tmp $XPM
	}
	fi
done
