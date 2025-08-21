#include "view3d.h"

view3D::view3D(QWidget *parent) : QOpenGLWidget(parent)
{
	QSurfaceFormat format;
	format.setMajorVersion(2);
	format.setMinorVersion(1);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	this->setFormat(format);
}

view3D::~view3D()
{
}

void view3D::initializeGL()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHTING);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);
}

void view3D::resizeGL(int width, int height)
{
	glWidth = width;
	glHeight = height;

	glViewport(0, 0, glWidth, glHeight);
	updateView();
	glLoadIdentity();
	gluLookAt(cameraPosX, cameraPosY, cameraPosZ, cameraLookAtX, cameraLookAtY, cameraLookAtZ, 0, 1, 0);
}

void view3D::paintGL()
{
	updateView();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Zoom
	glScaled(zoomScale, zoomScale, zoomScale);
	zoomScale = 1;

	// Rotation
	//glRotatef(rotX / 64.0, 1.0, 0.0, 0.0);
	glRotatef(rotY / 32.0, 0.0, 1.0, 0.0);
	//glRotatef(rotX / -128.0, 0.0, 0.0, 1.0);

	// Translation
	glTranslatef(posX, posY, posZ);

	if (showGrid)
		drawGrid(gridHalfGridCount);

	if (showObject)
	{
		drawObject(objectSizeX, objectSizeY, objectSizeZ, true);
		drawObject(objectSizeX, objectSizeY, objectSizeZ, false);
	}

	drawSpindle(10, 10, 10);

	if (showAxis)
		drawAxis();
}

void view3D::wheelEvent(QWheelEvent *event)
{
	// Zoom
	QPoint numDegrees = event->angleDelta();
	if (numDegrees.y() < 0)
	{
		zoomScale = 0.9;
	}
	else if (numDegrees.y() > 0)
	{
		zoomScale = 1.1;
	}
	this->update();
}

void view3D::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		leftClicked = true;
	}
	else if (event->button() == Qt::RightButton)
	{
		rightClicked = true;
	}

	event->accept();

	//qDebug() << leftClicked << rightClicked;
}

void view3D::mouseReleaseEvent(QMouseEvent *event)
{
	rotX = 0;
	rotY = 0;
	rotZ = 0;

	if (event->button() == Qt::LeftButton)
	{
		leftClicked = false;
	}
	else if (event->button() == Qt::RightButton)
	{
		rightClicked = false;
	}

	event->accept();

	//qDebug() << leftClicked << rightClicked;
}

void view3D::mouseMoveEvent(QMouseEvent *event)
{
	if (leftClicked)
	{
		float deltaY = event->x() - lastMousePosX;
		float deltaX = event->y() - lastMousePosY;
		rotX = deltaX * 10;
		rotY = deltaY * 10;

		this->update();
	}

	if (rightClicked)
	{
	}

	lastMousePosX = event->pos().x();
	lastMousePosY = event->pos().y();

	event->accept();

	//qDebug() << lastMousePosX << lastMousePosY;
}

void view3D::clearAllGCode()
{

}

void view3D::addGCode(QString gcode)
{
	if (gcode.startsWith("G21"))
	{
		isMetric = true;
	}
	else if (gcode.startsWith("G90"))
	{
		isAbsolutePositioning = true;
	}
	else if (gcode.startsWith("G1"))
	{
	}
}

void view3D::enablePerspective(bool enabled)
{
	isPersp = enabled;
	this->update();
}

void view3D::updateView()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (isPersp)
		gluPerspective(45, (float) glWidth / glHeight, 0.01, 10000.0);
	else
		glOrtho(-0.5 * 1000 * zoomScale, 0.5 * 1000 * zoomScale, -0.5 * glHeight / glWidth * 1000 * zoomScale, 0.5 * glHeight / glWidth * 1000 * zoomScale, -5 * 1000, 5 * 1000);
	glMatrixMode(GL_MODELVIEW);
}

void view3D::drawGrid(int halfGridSize)
{
	glPushMatrix();

	glBegin(GL_LINES);
	glColor3f(0.75f, 0.75f, 0.75f);
	for(int i =- halfGridSize; i <= halfGridSize; i++)
	{
		glVertex3f((float)i * 10 * gridSquareSize, 0, (float)-halfGridSize * 10 * gridSquareSize);
		glVertex3f((float)i * 10 * gridSquareSize, 0, (float)halfGridSize * 10 * gridSquareSize);

		glVertex3f((float)-halfGridSize * 10 * gridSquareSize, 0, (float)i * 10 * gridSquareSize);
		glVertex3f((float)halfGridSize * 10 * gridSquareSize, 0, (float)i * 10 * gridSquareSize);
	}
	glEnd();

	glPopMatrix();
}

void view3D::setDrawGrid(bool show)
{
	showGrid = show;
	this->update();
}

void view3D::drawObject(float x, float y, float z, bool isSurface)
{
	glPushMatrix();

	// If is drawing surface (not line)
	if (isSurface)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (isSurface)
	{
		glBegin(GL_QUADS);
	}
	else
	{
		glBegin(GL_LINES);
	}

	// Top
	if (isSurface)
	{
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
	}

	glVertex3f(-x, z + z, y);
	glVertex3f(x, z + z, y);
	glVertex3f(x, z + z, -y);
	glVertex3f(-x, z + z, -y);

	// Front
	if (isSurface)
	{
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
	}

	glVertex3f(x, -z + z, y);
	glVertex3f(x, z + z, y);
	glVertex3f(-x, z + z, y);
	glVertex3f(-x, -z + z, y);

	// Right
	if (isSurface)
	{
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
	}

	glVertex3f(x, z + z, -y);
	glVertex3f(x, z + z, y);
	glVertex3f(x, -z + z, y);
	glVertex3f(x, -z + z, -y);

	// Left
	if (isSurface)
	{
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
	}

	glVertex3f(-x, -z + z, y);
	glVertex3f(-x, -z + z, -y);
	glVertex3f(-x, z + z, -y);
	glVertex3f(-x, z + z, y);

	// Bottom
	if (isSurface)
	{
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
	}

	glVertex3f(x, -z + z, y);
	glVertex3f(-x, -z + z, y);
	glVertex3f(-x, -z + z, -y);
	glVertex3f(x, -z + z, -y);

	// Back
	if (isSurface)
	{
		glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
		glNormal3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 0.0f);
	}

	glVertex3f(x, z + z, -y);
	glVertex3f(x, -z + z, -y);
	glVertex3f(-x, -z + z, -y);
	glVertex3f(-x, z + z, -y);

	glEnd();

	if (isSurface)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
	}

	glPopMatrix();
}

void view3D::setDrawObject(bool show)
{
	showObject = show;
	this->update();
}

void view3D::drawAxis()
{
	glPushMatrix();

	glLineWidth(2.0);
	glDisable(GL_DEPTH_TEST);

	// Draw lines
	glBegin(GL_LINES);

	// X axis is red.
	glColor3f(30 * axisSize, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(30 * axisSize, 0, 0);

	// Z axis is green.
	glColor3f(0, 0, 30 * axisSize);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 30 * axisSize, 0);

	// Y axis is blue.
	glColor3f(0, 30 * axisSize, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 30 * axisSize);

	glEnd();

	// Draw text
	GLdouble projMatrix[16];
	GLdouble modelMatrix[16];
	GLint viewPort[4];
	GLdouble wx;
	GLdouble wy;
	GLdouble wz;

	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	QPainter painter(this);
	painter.setFont(QFont("Arial", 16));

	gluProject(30 * axisSize + 20, 0, 0, modelMatrix, projMatrix, viewPort, &wx, &wy, &wz);
	painter.setPen(Qt::red);
	painter.drawText(wx - 4, this->height() - wy + 4, "x");

	gluProject(0, 30 * axisSize + 20, 0, modelMatrix, projMatrix, viewPort, &wx, &wy, &wz);
	painter.setPen(Qt::blue);
	painter.drawText(wx - 4, this->height() - wy + 4, "z");

	gluProject(0, 0, 30 * axisSize + 20, modelMatrix, projMatrix, viewPort, &wx, &wy, &wz);
	painter.setPen(Qt::green);
	painter.drawText(wx - 4, this->height() - wy + 4, "y");

	painter.end();

	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0);

	glPopMatrix();
}

void view3D::setDrawAxis(bool show)
{
	showAxis = show;
	this->update();
}

void view3D::drawSpindle(float radius, float halfLength, int slices)
{
	//glPushMatrix();

	for(int i = 0; i < slices; i++)
	{
		float theta = ((float)i)*2.0*M_PI;
		float nextTheta = ((float)i+1)*2.0*M_PI;

		glBegin(GL_TRIANGLE_STRIP);
		// vertex at middle of end
		glVertex3f(0.0, halfLength, 0.0);
		// vertices at edges of circle
		glVertex3f(radius*cos(theta), halfLength, radius*sin(theta));
		glVertex3f (radius*cos(nextTheta), halfLength, radius*sin(nextTheta));
		// the same vertices at the bottom of the cylinder
		glVertex3f (radius*cos(nextTheta), -halfLength, radius*sin(nextTheta));
		glVertex3f(radius*cos(theta), -halfLength, radius*sin(theta));
		glVertex3f(0.0, -halfLength, 0.0);
		glEnd();
	}

	//glPopMatrix();
}

void view3D::updateGrid(int squareSize, int halfGridCount)
{
	gridSquareSize = squareSize;
	gridHalfGridCount = halfGridCount;
	this->update();
}

void view3D::updateObject(int sizeX, int sizeY, int sizeZ)
{
	objectSizeX = sizeX;
	objectSizeY = sizeY;
	objectSizeZ = sizeZ;
	this->update();
}

void view3D::updateAxis(int size)
{
	axisSize = size;
	this->update();
}
