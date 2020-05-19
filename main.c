#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <fcntl.h>

//#include "LinkList_MK.c"
#ifndef UNTITLED_LINKLIST_MK_H
#define UNTITLED_LINKLIST_MK_H

#include "LinkList_MK.h"
#endif //UNTITLED_LINKLIST_MK_H

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define BSPSIZE (unsigned)sizeof(BSP) // size a the struct node
#define max(a,b) (a>b)? a:b
// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")
extern struct node* head ;
extern struct node *last;
extern void insertLast( BSP data);
extern void insertFirst( BSP data);
extern struct node* node_delete(int key_pos_to_del);
extern pid_t findPidInList(int location);
extern struct node* node_delete_by_pid(int pid_to_del);
extern int length();

void refrences(char **pString);

// Greeting shell during startup
void init_shell()
{
    stifle_history(10);
    BSP dummy={.pid=getpid(),.status="dummy",.command="dummy"};
    insertFirst(dummy);
    clear();
    printf("\n\n\n\n********"
           "********");
    printf("\n\n\n\t***KOBI RULES*");
    printf("\n\n\t-MOSHE IS OK TOO-");
    printf("\n\n\n\n*******"
           "*********");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    clear();
}

// Function to take input
int takeInput(char* str)
{
    char* buf;

    buf = readline("\n>>> ");
    if (strlen(buf) != 0) {


        char i_indicator[1];
        memcpy(i_indicator,buf,1);
        if(strcmp(i_indicator,"!")==0){
            buf = history_list()[atoi(buf+1)]->line;

        }

        strcpy(str, buf);
        add_history(buf);

        return 0;
    } else {
        return 1;
    }
}

// Function to print Current Directory.
void printDir()
{
    sleep(1);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s", cwd);
}

// Function where the system command is executed
void execArgs(char** parsed, short IsABgdCmd)
{

    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if(IsABgdCmd)
            setpgid(0,0);

        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {

        // waiting for child to terminate
        if(!IsABgdCmd) {
            wait(NULL);
            return;
        }
        char* new_comm =malloc(8);
        // int  new_pid=getpid();
        strcpy(new_comm,parsed[0]);
        BSP p={.pid=pid,.status="running",.command=new_comm};
        insertLast(p);


        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe, short inOutPipe) {
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;


    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }

    if (inOutPipe != 2) // in/out situation
    {
        if ((p1 = fork()) < 0)
            printf("\ncould not create P1");
        else if (p1 == 0) {
            if ((dup2(pipefd[inOutPipe], inOutPipe)) == -1)
                printf("\ndup2 failed in P1");
            else {
                if ((execvp(parsed[0], parsed)) == -1){
                    if(!inOutPipe){
                        char* txt = ".txt";
                        int fd = open(strncat(parsed[0], txt,4 ), O_CREAT | O_WRONLY | O_APPEND | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
                        char* buff[100];
                        read(pipefd[inOutPipe],buff,100);
                        write(fd, buff, 100);
                        close(fd);
                    } else
                        printf("\nP1 Could not execute command 2..");
                }
                exit(0);
            }
        } else { // parent
            if ((p2 = fork()) < 0)
                printf("\ncould not create P2");
            else if (p2 == 0) {
                if ((dup2(pipefd[!inOutPipe], !inOutPipe)) == -1)
                    printf("\ndup2 failed in P1");
                else {
                    if ((execvp(parsedpipe[0], parsedpipe)) == -1){
                        if(inOutPipe){
                            char* txt=".txt";
                            int fd = open(strncat(parsedpipe[0], txt,4 ), O_CREAT | O_WRONLY | O_APPEND | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                            char* buff[100];
                            read(pipefd[!inOutPipe],buff,100);
                            write(fd, buff, 100);
                            close(fd);
                        } else
                            printf("\nP2 Could not execute command 1..");
                    }
                    exit(0);
                }
            } else {//parent
                waitpid(-1, NULL, WNOHANG | WCONTINUED | WUNTRACED);
                waitpid(-1, NULL, WNOHANG | WCONTINUED | WUNTRACED);
                sleep(0.5);
//                wait(NULL);
//                wait(NULL);
            }
        }
    } else {

        p1 = fork();
        if (p1 < 0) {
            printf("\nCould not fork");
            return;
        }

        if (p1 == 0) {
            // Child 1 executing..
            // It only needs to write at the write end
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            if (execvp(parsed[0], parsed) < 0) {
                printf("\nCould not execute command 1..");
                exit(0);
            }
        } else {
            // Parent executing
            p2 = fork();

            if (p2 < 0) {
                printf("\nCould not fork");
                return;
            }

            // Child 2 executing..
            // It only needs to read at the read end
            if (p2 == 0) {
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                if (execvp(parsedpipe[0], parsedpipe) < 0) {
                    printf("\nCould not execute command 2..");
                    exit(0);
                }
            } else {
                // parent executing, waiting for two children
                waitpid(-1, NULL, WNOHANG | WCONTINUED | WUNTRACED);
                waitpid(-1, NULL, WNOHANG | WCONTINUED | WUNTRACED);
                sleep(0.5);
//                wait(NULL);
//                wait(NULL);
            }
        }
    }
}

// Help command builtin
void openHelp()
{
    puts("\n*WELCOME TO MY SHELL HELP*"
         "\nList of Commands supported:"
         "\n>cd < PATH >:"
         "\n>changes working directory"
         "\n>example: cd /Desktop/moshe/"
         "\n>--------"
         "\n>export:"
         "\n>appends/add an environment variable"
         "\n>example: export MESSI=king"
         "\n>--------"
         /*"\n>get:"
         "\n>return the value of the environment variable specified"
         "\n>example: get MESSI"
         "\n>--------"*/
         "\n>history:"
         "\n>displays the previously executed commands. - the default history buffer size is 10 instructions"
         "\n>- the default history buffer size is 10 instructions"
         "\n>"
         "\n>history -s < num >:"
         "\n>sets the buffer's size to num and displays the previously executed commands"
         "\n>example: history -s 5"
         "\n>--------"
         "\n>!<number>:"
         "\n>repeats the command listed under number in the list of previously executed commands provided by history command"
         "\n>example: !2"
         "\n>--------"
         "\n><command>&:"
         "\n>places the process to run in the background"
         "\n>example: ls$"
         "\n>--------"
         "\n>jobs:"
         "\n>displays a numbered list of the currently, running in the background, processes"
         "\n>--------"
         "\n>kill < num >:"
         "\n>terminates the process, with the coresponding number, in the list, generated from jobs"
         "\n>example: kill 1"
         "\n>--------"
         "\n>exit:"
         "\n>terminates the shell program."
         "\n>--------"
         "\n>"
         "\n>have fun!ction");

    return;
}

// func for jobs
void jobsFunc(int* status, pid_t ptd){
    int index=0;
    struct node* next = head->next;
    char jobBuff[60];
    if(ptd==0){
        printf("index     pid     status      job  \n");
        while(next!=NULL)   {
            sprintf(jobBuff, "%d:        %d   %s     %s\n", index, next->data.pid, next->data.status,
                    next->data.command);
            printf("%s", jobBuff);
            ++index;
            next=next->next;
        }
    }
}


// Function to execute builtin commands
int ownCmdHandler(char** parsed)
{
    int NoOfOwnCmds = 7, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* variable= malloc(MAXCOM);
    int running_processes;

    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "jobs";
    ListOfOwnCmds[4] = "kill";
    ListOfOwnCmds[5] = "history";
    ListOfOwnCmds[6] = "export";
    //ListOfOwnCmds[7] = "get";
    if (!parsed[0])
        return 1;

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }

    register HIST_ENTRY **the_list;
    register int k,j;
    char isbuff[] = "-s";
    pid_t ptd;
    int status;
    switch (switchOwnArg) {
        case 1:
            if(!(running_processes=length())){
                printf("\nGoodbye\n");
                exit(0);
            }else{
                printf("\nThere are still %d running background processes.\nTo exit KILL THEM ALL!!!", running_processes);
                return 1;
            }
        case 2:
            chdir(parsed[1]);
            return 1;
        case 3:
            openHelp();
            return 1;
        case 4:
            //jobs
            jobsFunc(&status,0);
            return 1;
        case 5:
            //kill
            ptd = findPidInList( 1+atoi(parsed[1]));
            if(ptd!=-1)
                kill(ptd, 9);
            //sleep(3);
            //jobsFunc(&status,ptd);
            return 1;
        case 6:
            // history
            if(parsed[1] && strcmp(parsed[1],  isbuff)==0)
                stifle_history(atoi(parsed[2]));
            //history_buff_size = atoi(parsed[2]);
            the_list = history_list ();
            if (the_list)
                j=0;
            for (k = 0 ; the_list[k] ; k++) {
                printf("%d: %s\n", j, the_list[k]->line);//max(history_length - history_buff_size,0)
                ++j;
            }
            return 1;
        case 7:
            // export to PATH
            strcpy (variable,parsed[1]);
            if (-1 == putenv(variable))
                printf("putenv failed \n");
            return 1;
            /* case 8:
                 // get PATH
                 if (parsed[1])
                     if (getenv(parsed[1]))
                         strcpy (variable,getenv(parsed[1]));
                     else
                         variable="no such parameter";
                 printf("%s: %s\n",variable,parsed[1]);
                 return 1;*/
        default:
            break;
    }

    return 0;
}

// function for finding pipe
int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 3; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char* str, char** parsed)
{
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

int find_references(char **pString) {
    int i;
    char* found=malloc(MAXCOM);
    for (i = 0; i < MAXLIST && pString[i]; i++) {
        found=strchr(pString[i],'$');
        if (found==pString[i]) {
            removeChar(pString[i], '$');
            if (getenv(pString[i]))
                pString[i] = getenv(pString[i]);
            else
                pString[i] =NULL;
        }

    }
    return 1;
}

int processString(char* str, char** parsed, char** parsedpipe)
{


    char* strpiped[2];
    int piped = 0;

    piped = parsePipe(str, strpiped);

    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);

    } else {

        parseSpace(str, parsed);
    }

    if(!find_references(parsed)){
        printf("There is no such paramter");
    }



    if (ownCmdHandler(parsed))
        return 0;
    else
        return 1 + piped;
}


int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();
    short IsABgdCmd = 0;
    short inOutPipe;
    int t=0;
    char* std_in;
    char* std_out;
    int status;
    pid_t pid;
    struct node* next = head->next;


    while (1) {
        // print shell line
        printDir();
        IsABgdCmd = 0;
        inOutPipe = 2;
        if (takeInput(inputString)) // take input
            continue;
        t=0;

        std_in=strchr(inputString,'>');
        if (std_in) {
            *std_in = '|';
            inOutPipe = 1;
        }

        std_out=strchr(inputString,'<');
        if (std_out) {
            *std_out = '|';
            inOutPipe = 0;
        }

        while(inputString[t])
            t++;

        if( strcmp( &inputString[t-1],"&")==0) {
            IsABgdCmd = 1;
            inputString[t-1] ='\0';
        }


        while (((pid = waitpid(-1, &status, WNOHANG | WCONTINUED | WUNTRACED)) > 0)) {
            if (WIFEXITED(status))//
                node_delete_by_pid(pid);
            else if (WIFSTOPPED(status))
                next->data.status = "Stopped";
            else if (WIFCONTINUED(status))
                next->data.status = "running";
            else if (WIFSIGNALED(status))
                node_delete_by_pid(pid);

        }

        // process
        execFlag = processString(inputString,
                                 parsedArgs, parsedArgsPiped);

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs,IsABgdCmd);

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped,inOutPipe);

    }
    return 0;
}
