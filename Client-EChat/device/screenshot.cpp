#include "screenshot.h"

ScreenShot::ScreenShot(QWidget *parent)
    : QMainWindow(parent), isMousePressed(false), startPos(1,1), endPos(1,1)
{
//    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    // 设置窗口属性
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    screenPicture = new QPixmap();
    screenMenu = new QMenu(this);
    screenMenu->addAction("完成", this,[=]{
        // 获取截屏内容
        QRect rect(getCapturedRect(startPos, endPos));
        QPixmap pixmap = screenPicture->copy(rect);
        emit screenImage(pixmap);

        this->hide();
        clearScreenCaptureInfo();
    });
    screenMenu->addAction("保存", this,[=]{
        QRect rect(getCapturedRect(startPos, endPos));
        QString fileName = SCREENSHOT_PATH(selfId) + "/EC截图" + CURRENT_DATETIME + ".png";
        qDebug()<<fileName;
        screenPicture->copy(rect).save( fileName , "png");
        this->hide();
        clearScreenCaptureInfo();
    });
    screenMenu->addAction("保存全屏", this,[=]{
        QString fileName = SCREENSHOT_PATH(selfId) + "/EC截图" + CURRENT_DATETIME + ".png";
        screenPicture->save( fileName , "png");
        this->hide();
        clearScreenCaptureInfo();
    });
}

ScreenShot::~ScreenShot()
{
    if(screenPicture != nullptr){
        delete screenPicture;
    }
}

void ScreenShot::showEvent(QShowEvent *)
{
    QSize desktopSize = QGuiApplication::primaryScreen()->size();
    QScreen *pscreen = QApplication::primaryScreen();
    // 创建并返回一个像素图，该像素图是通过抓取受QRect（x，y，width，height）限制的给定窗口的内容而构建的。如果window为0，则会抓取整个屏幕。
    *screenPicture = pscreen->grabWindow(0, 0, 0, desktopSize.width(), desktopSize.height());

    QPixmap pix(desktopSize.width(), desktopSize.height());
    pix.fill((QColor(0, 0, 0, 150)));
    backgroundPicture = new QPixmap(*screenPicture);
    QPainter painter(backgroundPicture);
    painter.drawPixmap(0, 0, pix);
}

QRect ScreenShot::getCapturedRect(QPoint startpos, QPoint endpos)
{
    if(startpos == endpos){
        return QRect();
    }
    QPoint tmpTopLeftPos = startpos, tmpBottomRightPos = endpos;
    if(endpos.x() < startpos.x()){
        tmpBottomRightPos.setX(startpos.x());
        tmpTopLeftPos.setX(endpos.x());
    }
    if(endpos.y() < startpos.y()){
        tmpBottomRightPos.setY(startpos.y());
        tmpTopLeftPos.setY(endpos.y());
    }
    return QRect(tmpTopLeftPos, tmpBottomRightPos);
}

void ScreenShot::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawPixmap(0, 0, *backgroundPicture);

    QRect rect(getCapturedRect(startPos, endPos));
    if (rect.isValid()) {
        painter.drawPixmap(rect.x(), rect.y(), screenPicture->copy(rect));
    }
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawText(rect.x(), rect.y() - 8, tr("%1 x %2 px  |  (Enter快捷完成截图，鼠标右击更多操作！)")
                     .arg(rect.width())
                     .arg(rect.height()));
}

void ScreenShot::mouseMoveEvent(QMouseEvent *event)
{
    if(isMousePressed){
        QPoint tmpPos = event->pos();
        endPos = tmpPos;
        this->update();
    }
}

void ScreenShot::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        isMousePressed = true;
        startPos = event->pos();
    }
}

void ScreenShot::mouseReleaseEvent(QMouseEvent *) { isMousePressed = false; }

void ScreenShot::contextMenuEvent(QContextMenuEvent *)
{
    this->setCursor(Qt::ArrowCursor);
    screenMenu->exec(cursor().pos());
}

void ScreenShot::clearScreenCaptureInfo() { startPos = endPos; }

void ScreenShot::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        this->hide();
        clearScreenCaptureInfo();
    }else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        // 获取截屏内容
        QRect rect(getCapturedRect(startPos, endPos));
        QPixmap pixmap = screenPicture->copy(rect);
        emit screenImage(pixmap);

        this->hide();
        clearScreenCaptureInfo();
    }
}
