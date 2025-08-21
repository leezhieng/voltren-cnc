#ifndef VIEW3D_H
#define VIEW3D_H

#include <QDebug>
#include <QWidget>
#include <QPainter>

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <GL/glu.h>
#include <GL/gl.h>
#include <QtMath>

#include <QWheelEvent>

class view3D : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit view3D(QWidget *parent = 0);
	~view3D();

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

public:
	void clearAllGCode();
	void addGCode(QString gcode);

	void enablePerspective(bool enabled);
	void updateView();

	void drawGrid(int halfGridSize);
	void setDrawGrid(bool show);
	void drawObject(float x, float y, float z, bool isSurface);
	void setDrawObject(bool show);
	void drawAxis();
	void setDrawAxis(bool show);
	void drawSpindle(float radius, float halfLength, int slices);

	int gridSquareSize = 10;
	int gridHalfGridCount = 5;
	void updateGrid(int squareSize, int halfGridCount);

	int objectSizeX = 100;
	int objectSizeY = 100;
	int objectSizeZ = 100;
	void updateObject(int sizeX, int sizeY, int sizeZ);

	int axisSize = 10;
	void updateAxis(int size);

private:
	bool showGrid = true;
	bool showObject = true;
	bool showAxis = true;

	// Camera properties
	bool isPersp = true;
	float cameraPosX = 300;
	float cameraPosY = 600;
	float cameraPosZ = 900;
	float cameraLookAtX = 0;
	float cameraLookAtY = 0;
	float cameraLookAtZ = 0;

	// For calculating view
	float zoomScale = 1;
	bool leftClicked = false;
	bool rightClicked = false;
	float rotX = 0;
	float rotY = 0;
	float rotZ = 0;
	float posX = 0;
	float posY = 0;
	float posZ = 0;
	float lastMousePosX = 0;
	float lastMousePosY = 0;
	int glWidth = 0;
	int glHeight = 0;

	bool isMetric = true;
	bool isAbsolutePositioning = true;
	QVector<QString> commandBuffer;
};

#endif // VIEW3D_H
