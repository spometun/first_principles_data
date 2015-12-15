#include "Types.h"

#include "SimpleLog.h"

Node Source::getNext()
{
    Node res;
    bool ok = getNextOrFail(res);
    ASSERT(ok, "Internal error (unexpected end of file?)");
    return res;
}
