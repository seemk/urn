list(APPEND urn_sources
  urn/__bits/lib.hpp
  urn/__bits/platform_sdk.hpp
  urn/intrusive_stack.hpp
  urn/mutex.hpp
  urn/relay.hpp
)

list(APPEND urn_unittests_sources
  urn/common.test.hpp
  urn/common.test.cpp
  urn/intrusive_stack.test.cpp
  urn/mutex.test.cpp
  urn/relay.test.cpp
)
