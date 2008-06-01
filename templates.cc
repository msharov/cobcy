#include <hash.h>
#include <llist.h>
#include <stack.h>
#include <queue.h>
#include "symbase.h"
#include "symdata.h"
#include "symfile.h"
#include "symlabel.h"
#include "sympic.h"
#include "symrec.h"
#include "symvar.h"
#include "symscreen.h"

template class Chain<CobolData>;
template class Chain<CobolFile>;
template class Chain<CobolLabel>;
template class Chain<CobolRecord>;
template class Chain<CobolScreenField>;
template class Chain<StackEntry>;
template class ChainLink<CobolData>;
template class ChainLink<CobolFile>;
template class ChainLink<CobolLabel>;
template class ChainLink<CobolRecord>;
template class ChainLink<CobolScreenField>;
template class ChainLink<StackEntry>;
template class HashTable<CobolSymbol>;
template class LList<CobolData>;
template class LList<CobolScreenField>;
template class Queue<CobolFile>;
template class Queue<CobolLabel>;
template class Queue<StackEntry>;
template class Stack<CobolRecord>;
template class Stack<StackEntry>;
