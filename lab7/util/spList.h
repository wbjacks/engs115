// Module to contain returned subproblem values from a calc() method

void *spLOpen();

void spLClose(void *list);

int spLHasSubprob(void *list);

void *spLGetSubprob(void *list);

void *spLGetOrigProb(void *list);

int spLAddProb(void *list, void *prob, size_t size, int is_subprob);
