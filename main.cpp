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
  initscr();
  int rows, cols;
  getmaxyx(stdscr,rows,cols);
  /*editorwin is the subwindow that is derivative of the main window (stdscr).
I set the maximum height of editorwin to be the height of the main window (rows) I subtracted 6 from rows to allow for some space at the bottom for the filename, so the max heightis actually is rows-6. the width of the editorwin is the same as the main window, and the editorwin itself starts at the 3rd row from the first row of the main screen
  */
  WINDOW * editorwin = derwin(stdscr, (rows-6), cols, 3, 0); 
  mvprintw(0,0,"F1: Menu");
  mvprintw(0,cols/2,"CSCI 1730 Text Editor"); //cols/2 puts the heading in the center
  wborder(editorwin, '|', '|', '-', '-', '|', '|', '|', '|'); //this simply puts a border around editorwin like Mike has in the pdf. I'm considering creating a subwindow that is just the border, and then putting the editorwin in that subwindow. This would prevent accidentally overwriting the border
  mvwprintw(editorwin,1,1, "Testing."); // I input this string at 1,1 because 0,0 is actually the top-left character of the border.
  //working on implementing the cursor and inputting the characters
  refresh();
  getch();
  endwin();
  return 0;
}
