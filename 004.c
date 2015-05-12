#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

#include "hdfs.h"

int main(int argc, char *argv[])
{

    hdfsFileInfo *fi, *fip;
    hdfsFS fs;
    int fnum;
    int i;
    int row, col;
    char *defaultPath = ".";
    char *targetPath;
    int ch;
    char cwdBuffer[1024];
    char *cp;
    char prevBuffer[1024];
    int curr, page;
    WINDOW *win;

    initscr();		/* Start curses mode	      */
    clear();
    noecho();
    cbreak();
    getmaxyx(stdscr, row, col);

    if (col < 100) {
	endwin();
	printf("화면 폭이 너무 좁음. 니 폭은 %d, 나는 100이상 실행됨\n", col);
	exit(-1);
    }

    if (argc <= 1) {
	targetPath = defaultPath;
    }

    win = newwin(row, col, 0, 0);
    keypad(win, TRUE);

    fs = hdfsConnect("default", 0);
    fi = hdfsListDirectory(fs, targetPath, &fnum);
    curr = 0;

    wclear(win);
    page = 0;
    while(1) {
	
	cp = hdfsGetWorkingDirectory(fs, cwdBuffer, 1023);
	mvwprintw(win, 1, 0, "file count = %d, scr row = %d\n", fnum, row);
	mvwprintw(win, 2, 0, "working dir = %s\n", cp);
  
	if (!fnum) {
	    mvwprintw(win, 0, 0, "no file");
	}

	for (i = 0; (page < fnum / (row - 3) && i < row - 3) || (page == fnum / (row - 3) && i < fnum % (row - 3)); i++) {
		if (i == curr) {
		    wattron(win, A_REVERSE);
		    mvwprintw(win, i + 3, 0, "%04d:%s\n", page * (row - 3) + i, (fi + page * (row - 3) + i)->mName);
		    wattroff(win, A_REVERSE);
		} else {
		    mvwprintw(win, i + 3, 0, "%04d:%s\n", page * (row - 3) + i, (fi + page * (row - 3) + i)->mName);
		}
        }
        wrefresh(win);
	ch = wgetch(win);
    
	if (ch == 'q') break;
	else if (ch == KEY_LEFT) {
	    if (hdfsSetWorkingDirectory(fs, "../") != -1) {
		fi = hdfsListDirectory(fs, "./", &fnum);
		strncpy(prevBuffer, cwdBuffer, 1023);
		curr = 0;
		for (i = 0; i < fnum; i++) {
		    if (strncmp(prevBuffer, (fi + i)->mName, 1023)) {
			curr++;
		    } else {
			break;
		    }
		}
		page = curr / (row - 3);
		curr = curr % (row - 3);
	    } 	    
	    wclear(win);
	} else if (ch == KEY_DOWN) {
	    if (page < fnum / (row - 3)) { 
		if (curr < row - 3 - 1) {
		    curr++;
		} else {
		    curr = 0;
		    page++;
		    wclear(win);
		}
	    } else {
		if (curr < fnum % (row - 3) - 1 ) curr++;
	    }
	} else if (ch == KEY_UP) {
	    if (curr != 0) {
		curr--;
	    } else {
		if (page > 0) {
		    page--;
		    curr = (row - 3) - 1;
		    wclear(win);
		}
	    }


	} else if (ch == KEY_RIGHT && fnum) {

	    if ((fi + (page * (row - 3)) + curr)->mPermissions & 0400) {

		if ((fi + (page * (row - 3)) + curr)->mKind == 'F') {
		    clrtoeol();
		    refresh();
		    endwin();
		    fprintf(stderr, "hadoop fs -tail %s\n", (fi + (page * (row - 3)) + curr)->mName);
		    exit(0);
		}

        	strncpy(cwdBuffer, (fi + (page * (row - 3)) + curr)->mName, 1023);
		targetPath = cwdBuffer;
		curr = 0;
		page = 0;
		if (hdfsSetWorkingDirectory(fs, targetPath) != -1) {
		    fi = hdfsListDirectory(fs, targetPath, &fnum);
		} else {
                    wclear(win);
		    wattron(win, A_REVERSE);
		    mvwprintw(win, 0, 0, "open Error");
  		    wattroff(win, A_REVERSE);

		}
		wclear(win);
	    } else {
		wclear(win);
		wattron(win, A_REVERSE);
		mvwprintw(win, 0, 0, "permission X = %o", (fi + (page * (row - 3)) + curr)->mPermissions & 0400);
		wattroff(win, A_REVERSE);
	    }
	} else if (ch == KEY_HOME) {
	    curr = 0;
	    page = 0;
	    wclear(win);
	} else if (ch == KEY_END) {
	    curr = fnum % (row - 3) - 1;
	    page = fnum / (row - 3);
	    wclear(win);
	} else if (ch == KEY_NPAGE) {
	    if (page < fnum / (row - 3)) {
		page++;
		if (page == fnum / (row - 3)) {
		    if (curr > fnum % (row -3)) {
			curr = fnum % (row - 3) - 1;
		    }
		}
		wclear(win);
	    }
	} else if (ch == KEY_PPAGE) {
	    if (page > 0) {
		page--;
		wclear(win);
	    }
	}

    }
    clrtoeol();
    refresh();
    endwin();

    return 0;


#if 0
    const char* readPath = "/user/tadsvc/retargeting/mahout_input/000000_0";
    hdfsFile readFile = hdfsOpenFile(fs, readPath, O_RDONLY, 0, 0, 0);
    if(!readFile) {
        fprintf(stderr, "Failed to open %s for read!\n", readPath);
        exit(-1);
    }

    char buffer[1024];
    tSize num_read_bytes = hdfsRead(fs, readFile, (void*)buffer, 1000);

    hdfsCloseFile(fs, readFile);
#endif

}


