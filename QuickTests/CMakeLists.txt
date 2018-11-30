add_executable(CollectionsTest Data/CollectionsTest.cpp)
target_link_libraries(CollectionsTest GeneralEngine)

add_executable(PropertyTest Data/PropertyTest.cpp)
target_link_libraries(PropertyTest GeneralEngine)

add_executable(CoreTest Core/CoreTest.cpp)
target_link_libraries(CoreTest GeneralEngine)
