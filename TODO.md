- in_place.hpp uses std::in_place because eastl doesn't work.
- state_view.hpp doesn't work for reasons..._
- stateless_reference copy construtors should be deleted not defaulted._
- stack_core.hpp, eastl::max and eastl::max_element must be EA_CONSTEXPR'd
- eastl/allocator needs this :
```c+++
		#include <EASTL/utility.h>
		template <typename T, typename... Args>
		void allocator::construct(T* ptr, Args&&... args)
		{
			::new (static_cast<void*>(ptr)) T(eastl::forward<Args>(args)...);
		}

		template <typename T>
		void allocator::destroy(T* ptr)
		{
			ptr->~T();
		}
```
- eastl/internal/config. needs this:
```c++
	namespace eastl
	{
		using size_t = eastl_size_t;
		using ssize_t = eastl_ssize_t;
	}
```

- containers with pairs, customization_to_table needs fixing.