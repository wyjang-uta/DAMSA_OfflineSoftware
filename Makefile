# 컴파일러 설정
CXX := g++
CXXFLAGS := $(shell root-config --cflags)
LDFLAGS := $(shell root-config --glibs)

# 실행 파일 이름
TARGET := dmssoft

# 소스 및 객체 파일
SRCS := main.cxx DMSPlotter.cxx DMSMathUtils.cxx DMSSoft_Dict.cxx
OBJS := $(SRCS:.cxx=.o)

# 빌드 규칙
all: $(TARGET)

# Dictionary 파일 생성 규칙
DMSSoft_Dict.cxx: DMSPlotter.h DMSMathUtils.h LinkDef.h
	rootcling -f $@ -c $^

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 클린 빌드
clean:
	rm -f $(TARGET) $(OBJS) DMSPlotter_Dict.* DMSSoft_Dict_rdict.pcm

