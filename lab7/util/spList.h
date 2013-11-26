// Module to contain returned subproblem values from a calc() method

void *spLOpen();

void spLClose(void *list);

int spLHasSubprob(void *list);

void *spLGetProb(void *list, size_t *size);

void *spLGetSubprob(void *list, size_t *size);

void *spLGetOrigProb(void *list);

void spLAddProb(void *list, void *prob, size_t size, int is_subprob);
