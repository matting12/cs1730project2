#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <cerrno>
#include <unistd.h>
#include <iostream> 
#include <fstream>
#include <ncurses.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <curses.h>
#include <panel.h>
#include <form.h>
#include "methods.cpp"
using namespace std;



/* https://gist.github.com/alan-mushi/c8a6f34d1df18574f643 *
   Ncurses is weird and fills the content in the field with leading and trailing spaces. This method from github deletes the extra spaces. */
static char* trim_whitespaces(char *str);

/* Takes the error window and makes it into a form window; the message is the errno message from the open system call in main. Rows and cols variables are for resizing and scaling. This method will loop until a user enters a proper filename. Deletes itself and frees memory when the user name is correct. I tried to design this method to work with other system calls, so try it out with the write and read commands. */
char *  displayerror(WINDOW * editorwin, WINDOW * errorwin, char * message, int rows, int cols);

void f1Menu();

char * getcontents(int fd, WINDOW * editorwin, WINDOW * errorwin, char * message, int rows, int cols);

int main(const int argc, const char * argv []) {
  initscr();
  int screenrows, screencols;
  getmaxyx(stdscr,screenrows,screencols);
  /*boxwin is the subwindow that is derivative of the main window (stdscr).
I set the maximum height of boxwin to be the height of the main window (rows) I subtracted 6 from rows to allow for some space at the bottom for the filename, so the max heightis actually is rows-6, and did the same process for cols. The window starts at row 3 to allow for space between the header and window
  */
  WINDOW * boxwin = derwin(stdscr, (screenrows -6), screencols-6, 3,1);
  int boxrows, boxcols;
  getmaxyx(boxwin, boxrows, boxcols);
  /* Editorwin is the window where text should be placed and edited. It's a derivative window that starts 3 rows down from the box. */
  WINDOW * editorwin = derwin(boxwin, (boxrows-5), boxcols-5, 2, 2);
  int rows, cols;
  getmaxyx(editorwin,rows, cols); 
  mvprintw(0,0,"F1: Menu");
  mvprintw(0,cols/2,"CSCI 1730 Text Editor"); //cols/2 puts the heading in the center
  wborder(boxwin, '|', '|', '-', '-', '|', '|', '|', '|'); //this simply puts a border around boxwin, so that the text editor looks like Mike's in the pdf
  if(argc == 2){
    mvprintw(screenrows-2,0, argv[1]);
  } else {
    mvprintw(screenrows-2,0, "Untitled.txt"); //might change to the displayerror method
  }
  int fd = open(argv[1], O_RDWR);
  WINDOW * errorwin = newwin((rows-2), (cols-2),5,5);
  char * newfilename;
  if(fd == -1){
    newfilename = displayerror(editorwin, errorwin, (strerror(errno)), rows, cols); // will continously prompt the user until a correct username.
    mvprintw(screenrows-2, 0, newfilename);
    delwin(errorwin);
    fd = open(newfilename, O_RDWR);
  }
  mvprintw(screenrows-2,0, argv[1]);
  wmove(editorwin, 1, 1);
  editorfield[0] = new_field(rows-1, cols-5, 0,0,0,0);
  field_opts_off(editorfield[0], O_STATIC);
  field_opts_off(editorfield[0], O_BLANK);
  field_opts_on(editorfield[0], O_ACTIVE);
  FORM * editorform = new_form(editorfield);
  set_form_win(editorform, editorwin);
  WINDOW * fieldwin = derwin(editorwin, rows-1, cols-5, 1,1);
  set_form_sub(editorform, fieldwin);
  char * content = getcontents(fd, editorwin, errorwin, (strerror(errno)), rows, cols);
  set_field_buffer(editorfield[0], 0, content); 
  post_form(editorform);
  wrefresh(editorwin);
  refresh();
  keypad(editorwin, TRUE);
  idcok(editorwin, TRUE);
  noecho();
  int input;
 
  while((input = wgetch(editorwin))){ 
   //switch statement
	  switch(input){
			  
	case KEY_UP:
      	form_driver(editorform, REQ_UP_CHAR);
    	break;
    	case KEY_DOWN:
      	form_driver(editorform,REQ_DOWN_CHAR);
    	break;
 	case KEY_RIGHT:
      	form_driver(editorform,REQ_RIGHT_CHAR);
    	break;
	case KEY_LEFT:
      	form_driver(editorform,REQ_LEFT_CHAR);
    	break;
	case KEY_BACKSPACE:
      	form_driver(editorform, REQ_DEL_PREV);
    	break;
	case KEY_F(1):
      	f1Menu();
      	break;
	case 10:
      	form_driver(editorform, REQ_NEW_LINE);
    	break;
	case 9:
      	int space = 0;
      	while(space < 6){
		form_driver(editorform, REQ_INS_CHAR);
		space++;
      	}
    	break;
	  }
    if(input < 0401 && (input != 10 && input != 9)){ // 0401 is the starting octal values of special keys; anything less than this is normal characters like 'a' or 'f' and '!
      form_driver(editorform, input);
    }
  }
  endwin();
  return 0;
}
