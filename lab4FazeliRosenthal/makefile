# sample makefile for generic single-file compilations           
#                            
# NOTE: this file will work for both C and C++ programs.         
#  C programs (source code) should end with .c (lowercase)         
#  C++ programs (source code) should end with .C (uppercase) or .cc or .cpp    
#                            
# copy this file to your current directory, call it makefile or Makefile,    
# and type "make myprog" at the unix prompt              
#  -- this compiles the file "myprog.c" (or "myprog.C") and creates an     
#   executable called "myprog"                   
#                            
# to run your code, just type "myprog" (or "./myprog" if your current directory  
# is not on your path                      
#                            
# NOTE: if you want more information, do "man gcc" and/or "man make"       

CC = gcc    # use the gnu C compiler               
CFLAGS = -g   # C compilation flags: produce debugging info        
CFLAGS += -Wall # Warn about possible problems             
CFLAGS += -lm # link in math libraries               
#CFLAGS += -O # use instead of g for optimization            

#############################################################################    
# remove old backup files and core files               
clean:
	rm -i *~; rm -i core

