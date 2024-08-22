- in_place.hpp uses std::in_place because eastl doesn't work.
- BUGBUGBUG in stack_push.hpp about eastl::allocator...
_ BUGBUGBUG in raii.hpp for the same reason.
- state_view.hpp doesn't work for reasons..._
- stateless_reference copy construtors should be deleted not defaulted._