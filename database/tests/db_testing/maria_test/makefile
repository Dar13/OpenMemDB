CP = -classpath
JFLAGS = -g
JC = javac
JJ = java
CLASS_FILES = "."
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $(CP) $(CLASS_FILES) $*.java

CLASSES = \
    main.java \
    thread.java \
    connector.java \

default: classes

classes: $(CLASSES:.java=.class)

run:
	$(JJ) $(CP) $(CLASS_FILES) main

clean:
	$(RM) *.class
