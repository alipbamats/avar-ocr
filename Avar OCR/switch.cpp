#include "switch.h"

SwitchButton::SwitchButton(QWidget* parent)
    : QAbstractButton(parent), m_margin(3)
{
    setCheckable(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Initialize the slide animation
    m_animation = new QPropertyAnimation(this, "handleX", this);
    m_animation->setDuration(150); // Speed of the slide in milliseconds

    m_handleX = m_margin;
}

QSize SwitchButton::sizeHint() const {
    return QSize(55, 30); // Default optimal switch dimensions
}

QSize SwitchButton::minimumSizeHint() const {
    return QSize(45, 25);
}

int SwitchButton::targetHandleX(bool checked) const {
    int handleRadius = height() - (2 * m_margin);
    if (checked) {
        return width() - handleRadius - m_margin;
    }
    return m_margin;
}

void SwitchButton::resizeEvent(QResizeEvent* event) {
    QAbstractButton::resizeEvent(event);
    // Snap handle to the correct position when resized
    m_handleX = targetHandleX(isChecked());
}

void SwitchButton::nextCheckState() {
    // Override default toggle mechanism to insert smooth animation
    QAbstractButton::nextCheckState();

    m_animation->stop();
    m_animation->setStartValue(m_handleX);
    m_animation->setEndValue(targetHandleX(isChecked()));
    m_animation->start();
}

void SwitchButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Setup colors based on checked and enabled states
    QColor trackColor = isChecked() ? QColor(76, 217, 100) : QColor(220, 220, 220); // Green vs Light Grey
    QColor handleColor = Qt::white;

    if (!isEnabled()) {
        trackColor = trackColor.lighter(130);
        handleColor = QColor(240, 240, 240);
    }

    // 2. Draw the background track (Pill shape)
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawRoundedRect(rect(), height() / 2.0, height() / 2.0);

    // 3. Draw the sliding handle (Circle)
    int handleDiameter = height() - (2 * m_margin);
    painter.setBrush(handleColor);

    // Smooth drop shadow simulation around the circle handle
    painter.setPen(QPen(QColor(0, 0, 0, 30), 1));
    painter.drawEllipse(m_handleX, m_margin, handleDiameter, handleDiameter);
}