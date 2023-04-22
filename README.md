# OS_Ex2 

## Download & run the program 

  In order to run this project follow this steps:  
  1. Download zip from our reposetory : push on code -> Download zip.  
  2. Extract all on yor computer.  
  3. Open a new java project.  
  4. Open the file in the workspace - Ubuntu 20.04 LTS.   
  5. execute "make all"  
  6. run any of the following tools 
  

## cmp:
### in stshell:
cmp ˂filename1˃ ˂filename2˃ [-v] [-i]
### in other terminals:
./cmp ˂filename1˃ ˂filename2˃ [-v] [-i]

both files should exist on the disk.

## copy:
### in stshell:
copy ˂filename1˃ ˂filename2˃ [-v] [-f]
### in other terminals:
./copy ˂filename1˃ ˂filename2˃ [-v] [-f]

the first file should exist on the disk.
if the [-f] flag is not written, the second file should not exist on the disk.

## encode:
### in stshell:
encode ˂codecA/codecB˃ ˂message˃
### in other terminals:
./encode ˂codecA/codecB˃ ˂message˃

## decode: 
### in stshell: 
decode ˂codecA/codecB˃ ˂message˃
### in other terminals:
./decode ˂codecA/codecB˃ ˂message˃

## stshell:
  1. execute "./stshell"
  2. run any system command or one of the above tools  
  3. use a single ˃ or double ˃˃ redirection operators:
    a. <br /> command > filename
    b. <br /> command ˃˃ filename
  4. use a single | or double || pipeline operators: 
    a. <br />command1 | command2 
    b. <br />command1 | command2 | command3
  
