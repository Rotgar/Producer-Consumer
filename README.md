ProducerB adds 2 letters to buffer at once. A letter must be read by both conumsers to be removed. Number of letters in buffer always >= 3  
  
Example commands:    
  
 gcc main.c sem.c -o sem  
 ./sem  
  
Program ends with closing terminal.  
After ending it free allocated space by typing:  
  
ipcrm -a


