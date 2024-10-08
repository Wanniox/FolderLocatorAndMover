#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
    bool isSelected();
    void setStylesheet();
    void setSelectedNoSignal(bool select);
signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
private:
    bool selected = false;
};

#endif // CLICKABLELABEL_H
