#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream> 
#include <fstream>
#include <ncurses.h>
#include <vector>
#include <string>
#include <sstream>
#include <curses.h>
using namespace std;
 
int main(const int argc, const char * argv []) {
  ifstream ourfile(argv[1]);
  /* the following code reads the contents of a file into a string vector, whcih will be used to reflect the entire contents in later parts of the function. I based my approach off this reddit post: https://www.reddit.com/r/learnprogramming/comments/26fqly/c_read_text_file_line_by_line_into_a_vector/chqm6zf/. This accomplishes the task of reading a file into some time of dynamic array much easier, since vectors can allocate themselves with the push back function */
  vector<string> fileturnedarray;
  string lineinput;
  while(getline(ourfile, lineinput)){
    string linerow;
    linerow = lineinput + "\n";
    fileturnedarray.push_back(linerow);
  }
  initscr();
  unsigned int rows, cols;
  getmaxyx(stdscr,rows,cols);
  /*this loop prints from the beginnig of the file to the row in the file that matches the maximum rows allowed by the ncurses stdscr window. The loop iterator is unsigned to prevent a segmentation fault when trying to scroll up at the beginnig of the file.
   */
  for(unsigned long long int i = 0; i < rows; i++){ 
    const char * thisline = fileturnedarray[i].c_str();
    printw(thisline);
  }
  keypad(stdscr, TRUE);
  scrollok(stdscr,  TRUE);
  refresh();
  int input;
  unsigned long long int pos = 0;
  while((input = getch()) != 'q'){
    if(input == KEY_UP){
      pos = pos - 1;
      for(unsigned long long int i  = pos; i < rows+pos; i++){ 
        const char * thisline = fileturnedarray[i].c_str();
        printw(thisline);
      }
      refresh();
    }
    if(input == KEY_DOWN){
      pos = pos + 1;
      for(unsigned long long int i  = pos; i < rows+pos; i++){
        const char * thisline = fileturnedarray[i].c_str();
        printw(thisline);
      }
      refresh();
    }
  }
  endwin();
}
