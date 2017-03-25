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
using namespace std;
FIELD *field[1];

void wrap(int &pos1, int &pos2, int cols){
  if(pos2 >= cols){
    pos1++;
    pos2 = 1;
  }
}
void wrapback(int &pos1, int &pos2, int cols){
  if(pos2 <= 0){
    pos1--;
    pos2 = cols;
  }
}
/* https://gist.github.com/alan-mushi/c8a6f34d1df18574f643 *
   Ncurses is weird and fills the content in the field with leading and trailing spaces. This method from github deletes the extra spaces. */
static char* trim_whitespaces(char *str)
{
  char *end;

  // trim leading space
  while(isspace(*str))
    str++;

  if(*str == 0) // all spaces?
    return str;

  // trim trailing space
  end = str + strnlen(str, 128) - 1;

  while(end > str && isspace(*end))
    end--;

  // write new null terminator
  *(end+1) = '\0';

  return str;
}
/* Takes the error window and makes it into a form window; the message is the errno message from the open system call in main. Rows and cols variables are for resizing and scaling. This method will loop until a user enters a proper filename. Deletes itself and frees memory when the user name is correct. I tried to design this method to work with other system calls, so try it out with the write and read commands. */
char *  displayerror(WINDOW * editorwin, WINDOW * errorwin, char * message, int rows, int cols){
  //WINDOW *  errorwin = derwin(editorwin, (rows-2), (cols-2),1,1);
  int erows, ecols;
  getmaxyx(errorwin, erows, ecols);
  wborder(errorwin, '|', '|', '-', '-', '|', '|', '|', '|');
  mvwaddstr(errorwin, 2, 1, message);
  mvwaddstr(errorwin, 1, ecols/2, "Re-Enter Filename");
  //PANEL * errorpanel = new_panel(errorwin);

  field[0] = new_field(1, ecols-5, 0, 0, 0, 0);
  FORM * aform = new_form(field);
  int displayrows, displaycols;
  getmaxyx(errorwin, displayrows, displaycols);
  //scale_form(aform, &displayrows, &displaycols);
  //WINDOW *frmwin = newwin(rows -8, cols -8, 5, 5);
  //mvwaddstr(frmwin, 1, 1, "Re-Enter Filename");
  //keypad(frmwin, TRUE);
  set_form_win(aform, errorwin);
  WINDOW * fieldwin = derwin(errorwin, 4,ecols-3,3,3);
  set_form_sub(aform, fieldwin);
  post_form(aform);
  wrefresh(editorwin); 
  //update_panels();
  //doupdate();
  noecho();
  int ch;
  int fd = -1;
  char * filename;
  while(fd == -1){
    while((ch = wgetch(errorwin)) != 10){
      form_driver(aform, ch);
    }
    form_driver(aform, REQ_VALIDATION);
    filename = trim_whitespaces(field_buffer(field[0], 0));
    fd = open(filename, O_RDWR);
    if(fd == -1){
      wmove(errorwin, 2, 1);
      wclrtoeol(errorwin);
      mvwaddstr(errorwin, 2, 1, strerror(errno));
      form_driver(aform, REQ_CLR_FIELD);
    }
  }
  unpost_form(aform);
  free_form(aform);
  free_field(field[0]);
  delwin(fieldwin);
  delwin(errorwin); 
  return filename;
}
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
  wborder(boxwin, '|', '|', '-', '-', '|', '|', '|', '|'); //this simply puts a border around boxwin, so that the text editor looks like Mike's in the pdf.
  //mvwprintw(editorwin, 1,1, "Testing");
  int pos1, pos2;
  getyx(editorwin, pos1, pos2);
  if(argc == 2){
    mvprintw(screenrows-2,0, argv[1]);
  } else {
    mvprintw(screenrows-2,0, "Untitled.txt"); //might change to the displayerror method
  }
  int fd = open(argv[1], O_RDWR);
  if(fd == -1){
    WINDOW *  errorwin = newwin((rows-2), (cols-2),5,5);
    char * newfilename = displayerror(editorwin, errorwin, (strerror(errno)), rows, cols); // will continously prompt the user until a correct username.
    mvprintw(screenrows-2, 0, newfilename);
    delwin(errorwin);
  }
  refresh();
  scrollok(editorwin, TRUE);
  keypad(editorwin, TRUE);
  idcok(editorwin, TRUE);
  noecho();
  int input;
  /*Feel free to switch the following into a switch statement*/
  while((input = wgetch(editorwin))){ //I set tilda to stop the programming just for personal debugging, the final version won't have this.
    if(input == KEY_UP){
      pos1--;
      wmove(editorwin, pos1, pos2);
    }
    if(input == KEY_DOWN){
      pos1++;
      wmove(editorwin, pos1, pos2);
    }
    if(input == KEY_RIGHT){
      pos2++;
      wrap(pos1, pos2, cols);
      wmove(editorwin, pos1, pos2);
    }
    if(input == KEY_LEFT){
      pos2--;
      wrapback(pos1, pos2, cols);
      wmove(editorwin, pos1, pos2);
    }
    if(input == KEY_BACKSPACE){
      pos2--;
      wrapback(pos1, pos2, cols);
      mvwdelch(editorwin, pos1, pos2);
    }
    if(input == 10){
      char * restofline = (char *)malloc((cols+1)); //the char * pointer has to be allocated first before data can read into.
      memset(restofline, '\n', cols+1);
      winstr(editorwin, restofline);
      pos1++;
      wmove(editorwin, pos1, pos2);
      waddstr(editorwin,restofline);
      wmove(editorwin, pos1, pos2);
      for(unsigned int i = 0; i <= strlen(restofline); i++){
	mvwdelch(editorwin, pos1-1, (pos2));
      }
      //mvwinsch(editorwin,pos1,pos2, input);
      wmove(editorwin, pos1, pos2);
      free(restofline);
    }
    if(input == 9){
      // char * restofline = (char *)malloc((cols+1)); //the char * pointer has to be allocated first before data can read into.
      // memset(restofline, 0, cols+1);
      //winstr(editorwin, restofline);
      //pos2 += 6;
      //wrap(pos1, pos2, cols);
      mvwinsch(editorwin, pos1, pos2, input);
      //wmove(editorwin, pos1, pos2);
      //pos2 += 6;
      //wmove(editorwin, pos1, pos2);1
      /*for(unsigned int i = 0; i <= strlen(restofline); i++){
	pos2 -= 6;
	mvwdelch(editorwin, pos1, pos2);
	}*/
    }
    if(input < 0401 && (input != 10 && input != 9)){ // 0401 is the starting octal values of special keys; anything less than this is normal characters like 'a' or 'f' and '!
      wrap(pos1, pos2, cols);
      mvwinsch(editorwin, pos1, pos2, input);
      //char * saveme =  textwrap(pos1, pos2, cols, editorwin);
      //mvwinsstr(editorwin, pos1, pos2, saveme);
      pos2++;
    }
  }
  endwin();
  return 0;
}
