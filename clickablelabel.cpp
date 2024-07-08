#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
    selected = !selected;
    setStylesheet();
}

bool ClickableLabel::isSelected() {
    return selected;
}

void ClickableLabel::setSelectedNoSignal(bool select) {
    selected = select;
    setStylesheet();
}

void ClickableLabel::setStylesheet() {
    if (selected) {
        setStyleSheet("font-weight: bold;");
    } else {
        setStyleSheet("font-weight: normal;");
    }
}
