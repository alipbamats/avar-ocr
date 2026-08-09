#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QAbstractButton>
#include <QPropertyAnimation>
#include <QPainter>

class SwitchButton : public QAbstractButton {
    Q_OBJECT
        // Define an animation property for the sliding handle position
        Q_PROPERTY(int handleX READ handleX WRITE setHandleX)

public:
    explicit SwitchButton(QWidget* parent = nullptr);

    // Override sizing hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Getter and setter for the animation property
    int handleX() const { return m_handleX; }
    void setHandleX(int x) { m_handleX = x; update(); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void nextCheckState() override; // Handles the state toggling
    void resizeEvent(QResizeEvent* event) override;

private:
    int m_handleX;
    int m_margin;
    QPropertyAnimation* m_animation;

    int targetHandleX(bool checked) const;
};

#endif // SWITCHBUTTON_H