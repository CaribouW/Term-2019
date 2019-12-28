struct semaphore
{
    int value;
    semaphore(int value)
    {
        this->value = value;
    }
};
extern void P(semaphore *);
extern void V(semaphore *);