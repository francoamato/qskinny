TEMPLATE = subdirs

# c++
SUBDIRS += \
    desktop \
    grid-layout \
    horizontal-layout \
    layouts \
    listbox \
    messagebox \
    mycontrols \
    size-policies \
    sliders \
    stack-layout \
    thumbnails \
    tabview \
    vertical-layout

qtHaveModule(svg) {

    # when checking in qvg files we could drop the svg dependency 

    SUBDIRS += \
        automotive \
        qvgviewer
}

# qml
SUBDIRS += \
    boxes \
    buttons \
    colorswitch \
    frames \
    gbenchmark \
    glabels \
    messageboxQml \
    tlabels
