#!/bin/zsh

for files in $(ls); do

   if [ -d "$files" ] ; then 
      cd $files
	  zsh run.sh
	  cd ..
   fi

done