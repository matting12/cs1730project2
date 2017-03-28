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
FIELD * editorfield[1];

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
/* Takes the error window and makes it into a form window; the message is the e
rrno message from the open system call in main. Rows and cols variables are for
 resizing and scaling. This method will loop until a user enters a proper filen
ame. Deletes itself and frees memory when the user name is correct. I tried to
design this method to work with other system calls, so try it out with the write and read commands. */

char *  displayerror(WINDOW * editorwin, WINDOW * errorwin, char * message, int rows, int cols){
  // WINDOW *  errorwin = derwin(editorwin, (rows-2), (cols-2),1,1);            
  int erows, ecols;
  getmaxyx(errorwin, erows, ecols);
  wborder(errorwin, '|', '|', '-', '-', '|', '|', '|', '|');
  mvwaddstr(errorwin, 2, 1, message);
  mvwaddstr(errorwin, 1, ecols/2, "Re-Enter Filename");
  // PANEL * errorpanel = new_panel(errorwin);                                  

  field[0] = new_field(1, ecols-5, 0, 0, 0, 0);
  FORM * aform = new_form(field);
  set_form_win(aform, errorwin);
  WINDOW * fieldwin = derwin(errorwin, 4,ecols-5,3,3);
  set_form_sub(aform, fieldwin);
  post_form(aform);
  wrefresh(errorwin);
  // update_panels();                                                           
  // doupdate();                                                                
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
  close(fd);
  unpost_form(aform);
  free_field(field[0]);
  free_form(aform);
  delwin(fieldwin);
  delwin(errorwin);
  return filename;
}

char * getcontents(int fd, WINDOW * editorwin, WINDOW * errorwin, char * message, int rows, int cols){
  char * buffer = 0;
  off_t filesize = lseek(fd, 0, SEEK_END);
  if(filesize == -1){
    char * newfilename = displayerror(editorwin, errorwin, (strerror(errno)), rows, cols);
    open(newfilename, O_RDWR);
    filesize = lseek(fd, 0, SEEK_END);
  }
  int seekback = lseek(fd, 0, SEEK_SET);
  if(seekback == -1){
    char * newfilename = displayerror(editorwin, errorwin, (strerror(errno)), rows, cols);
    open(newfilename, O_RDWR);
    seekback = lseek(fd, 0, SEEK_SET);
  }

  buffer = (char*)malloc(filesize+1);
  buffer[filesize] = '\0';
  read(fd, buffer, filesize);
  return buffer;
}

void f1Menu(){
  WINDOW *w;
  char list[4][7] = { "Open", "Save", "SaveAs", "Exit" };
  char item[7];
  int ch, i = 0, width = 7;


  w = newwin( 10, 15, 1, 1 ); // create a new window                            
  box( w, 0, 0 ); // sets default borders for the window                        
  // now print all the menu items and highlight the first one                   
  for( i=0; i<4; i++ ) {
    if( i == 0 )
      wattron( w, A_STANDOUT ); // highlights the first item.                   
    else
            wattroff( w, A_STANDOUT );
    sprintf(item, "%-7s",  list[i]);
    mvwprintw( w, i+1, 2, "%s", item );
  }

  wrefresh( w ); // update the terminal screen                                  

  i = 0;
  noecho(); // disable echoing of characters on the screen                      
  keypad( w, TRUE ); // enable keyboard input for the window.                   
  curs_set( 0 ); // hide the default screen cursor.    
    curs_set( 0 ); // hide the default screen cursor.                             

  // get the input                                                              
  while(( ch = wgetch(w)) != 'q'){

    // right pad with spaces to make the items appear with even width.          
    sprintf(item, "%-7s",  list[i]);
    mvwprintw( w, i+1, 2, "%s", item );
    // use a variable to increment or decrement the value based on the input.   
    switch( ch ) {
    case KEY_UP:
      i--;
              i = ( i<0 ) ? 4 : i;
      break;
    case KEY_DOWN:
      i++;
      i = ( i>4 ) ? 0 : i;
      break;
    }
    // now highlight the next item in the list.                                 
    wattron( w, A_STANDOUT );

    sprintf(item, "%-7s",  list[i]);
   mvwprintw( w, i+1, 2, "%s", item);
    wattroff( w, A_STANDOUT );
  }

  delwin( w );
  endwin();
}


