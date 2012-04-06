set(tst_accessibilityclient_SRCS tst_accessibilityclient.cpp)

kde4_add_unit_test(tst_accessibilityclient ${tst_accessibilityclient_SRCS})

target_link_libraries(tst_accessibilityclient ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} kdeaccessibilityclient)
