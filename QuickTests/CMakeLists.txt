add_executable(CollectionsTest Data/CollectionsTest.cpp)
target_link_libraries(CollectionsTest GeneralEngine)

add_executable(TriggeredTest Data/TriggeredTest.cpp)
target_link_libraries(TriggeredTest GeneralEngine)

add_executable(CoreTest Core/CoreTest.cpp)
target_link_libraries(CoreTest GeneralEngine)

add_executable(MutexTest Data/MutexTest.cpp)
target_link_libraries(MutexTest GeneralEngine)
