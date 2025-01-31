# pidchain
-A simple program that finds the parent pids of each process from target to origin 0
pidchain is a really simple tool to find the parents of all processes from your target process to process 0

usage:
  pidchain -pid=234 #where 234 is the process you wish to get the pid chain for
  -s option supresses the text message
  -d=',' changes the delimiter to a comma

 example:
  user@system:!/prompt$ bin/pidchain -s -d=',' -pid=1210288
  1210288,1209762,287628,5045,1,0


  user@system:!/prompt$ bin/pidchain -pid=1210288
  pid chain is :1210288<-1209762<-287628<-5045<-1<-0
