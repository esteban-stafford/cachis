#ifndef WRITEPOLICY_H
#define WRITEPOLICY_H

enum write_policy {WRITE_THROUGH=0, WRITE_BACK=1};

void write_through();
void write_back();

#endif
