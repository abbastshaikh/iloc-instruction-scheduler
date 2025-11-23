#include <InternalRepresentation.hpp>
#include <DependenceGraph.hpp>

class Scheduler {
public:
    void schedule (InternalRepresentation& rep);

private:
    DependenceGraph buildDependenceGraph (const InternalRepresentation& rep);
};