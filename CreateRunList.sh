#!/bin/tcsh

set RunListFile=RunList.txt

@ i = 0
while ($i <= 5)
@ no = $i * 10000
echo "$no" >>  $RunListFile
@ i = $i + 1
end


