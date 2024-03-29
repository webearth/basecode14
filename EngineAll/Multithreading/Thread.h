/*
Thread.h
Written by Matthew Fisher

A thread is a single unit of code execution
*/
#pragma  once

struct Thread
{
public:
    Thread();
    ~Thread();
    
    void CloseThread();
    
    void Begin(LPTHREAD_START_ROUTINE StartFunction, void *Context);

private:
    HANDLE _Handle;
};
