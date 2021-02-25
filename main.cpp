//
//  Display a rotating cube 
//

#include "Angel.h"

typedef vec4  color4;
typedef vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];
int tracker[200][2];	//for solve
int track = 0;			//for solve
int co;					//selected face to be rotated
int tat;				//rotation direction
int lock;				//for random, solve
int counter = 0;		//for rotation progress calculation
int ran;				//random rotation count between 5, 10

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.2, -0.2,  0.2, 1.0),
	point4(-0.2,  0.2,  0.2, 1.0),
	point4(0.2,  0.2,  0.2, 1.0),
	point4(0.2, -0.2,  0.2, 1.0),
	point4(-0.2, -0.2, -0.2, 1.0),
	point4(-0.2,  0.2, -0.2, 1.0),
	point4(0.2,  0.2, -0.2, 1.0),
	point4(0.2, -0.2, -0.2, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.5, 0.0, 1.0),  // orange
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

mat4  projection;
mat4 model_view;
mat4 model_view1;
mat4 model_view2;
mat4 model_view3;
mat4 model_view4;
mat4 model_view5;
mat4 model_view6;
mat4 model_view7;
mat4 model_view8;
mat4 model_view9;
mat4 model_view10;
//2D array for rotation displacement
mat4 *Rubik[6][4] = { { &model_view1, &model_view2, &model_view3, &model_view4},{ &model_view5, &model_view6, &model_view7, &model_view8 },{ &model_view1, &model_view2, &model_view5, &model_view6 },
					{ &model_view3, &model_view4, &model_view7, &model_view8 },{ &model_view1, &model_view3, &model_view5, &model_view7 },{ &model_view2, &model_view4, &model_view6, &model_view8 } };

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

void
quad(int a, int b, int c, int d, int e)
{
	// Initialize colors

	colors[Index] = vertex_colors[e]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2, 1);
	quad(2, 3, 7, 6, 2);
	quad(3, 0, 4, 7, 3);
	quad(6, 5, 1, 2, 4);
	quad(4, 5, 6, 7, 5);
	quad(5, 4, 0, 1, 6);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	//initialize for random start
	ran = rand() % 5 + 5;
	co = rand() % 6 + 1;
	tat = rand() % 2 + 1;
	lock = 0;
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	// Set current program object
	glUseProgram(program);

	model_view = identity() * Scale(2.025, 2.025, 2.025); //for picking
	model_view1 = identity() *  Translate(0.205, 0.205, 0.205);
	model_view2 = identity() *  Translate(-0.205, 0.205, 0.205);
	model_view3 = identity() *  Translate(0.205, -0.205, 0.205);
	model_view4 = identity() *  Translate(-0.205, -0.205, 0.205);
	model_view5 = identity() *  Translate(0.205, 0.205, -0.205);
	model_view6 = identity() *  Translate(-0.205, 0.205, -0.205);
	model_view7 = identity() *  Translate(0.205, -0.205, -0.205);
	model_view8 = identity() *  Translate(-0.205, -0.205, -0.205);

	// Set projection matrix
	projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Ortho(): user-defined function in mat.h
														 //projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	// Enable hiddden surface removal
	glEnable(GL_DEPTH_TEST);

	// Set state variable "clear color" to clear buffer with.
	glClearColor(0.8, 0.8, 0.8, 1.0);

	std::cout << "2x2 Rubik Cube Commands: " << std::endl;//displays user commands
	std::cout << "Left Mouse Button: Select the face to be rotated (Also print selected faces initial position)." << std::endl;
	std::cout << "1, 2: Rotate the selected face clockwise/counterclockwise." << std::endl;
	std::cout << "W, A, S, D: Rotate the camera around the cube." << std::endl;
	std::cout << "R: Randomize the cube." << std::endl;
	std::cout << "E: Solves the cube." << std::endl;
	std::cout << "Q: Exit program." << std::endl;
	std::cout << "H: Print commands." << std::endl;
	std::cout << std::endl;
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view1);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view2);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view3);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view4);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view5);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view6);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view7);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view8);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glutSwapBuffers();

}

//---------------------------------------------------------------------
//
// reshape
//

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	if (w <= h)
		projection = Ortho(-1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w,
			1.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
	else  projection = Ortho(-1.0* (GLfloat)w / (GLfloat)h, 1.0 *
		(GLfloat)w / (GLfloat)h, -1.0, 1.0, -1.0, 1.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}
//For R button
void random() {
	if (lock==1) {
		ran = rand() % 5 + 5;
		lock = 0;
	}
}
//For E button
void solve() {
	if (lock==1) {
		track--;
		lock = 2;
	}
}


//----------------------------------------------------------------------------

void
idle(void)
{
	if (lock==0 && tat == 0) { //Randomizer
		if (ran == 0) { lock = 1; std::cout << "Randomizing complete!" << std::endl; std::cout << std::endl; }
		else {
			co = rand() % 6 + 1;
			tat = rand() % 2 + 1;
			ran--;
		}
	}
	if (lock == 2 && tat == 0) {	//Solver
		if (track == -1) { lock = 1; track = 0; std::cout << "Solving complete!" << std::endl; std::cout << std::endl; }
		else {
			co = tracker[track][0];
			tat = tracker[track][1];
			track--;
		}
	}
	Theta[0] = Theta[1] = Theta[2] = 0.1;
	if (co == 1 && tat == 1) {	//Rotation
		*Rubik[0][0] = RotateZ(Theta[Zaxis]) * *Rubik[0][0];
		*Rubik[0][1] = RotateZ(Theta[Zaxis]) * *Rubik[0][1];
		*Rubik[0][2] = RotateZ(Theta[Zaxis]) * *Rubik[0][2];
		*Rubik[0][3] = RotateZ(Theta[Zaxis]) * *Rubik[0][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][1];
			Rubik[4][1] = Rubik[3][1];
			Rubik[3][1] = Rubik[5][0];
			Rubik[5][0] = Rubik[2][0];
			Rubik[2][0] = temp;
			temp = Rubik[4][0];
			Rubik[4][0] = Rubik[3][0];
			Rubik[3][0] = Rubik[5][1];
			Rubik[5][1] = Rubik[2][1];
			Rubik[2][1] = temp;
			temp = Rubik[0][0];
			Rubik[0][0] = Rubik[0][2];
			Rubik[0][2] = Rubik[0][3];
			Rubik[0][3] = Rubik[0][1];
			Rubik[0][1] = temp;
			tat = 0; 
			counter = 0;
			if (lock != 2) {//For solver
				tracker[track][0] = co;
				tracker[track][1] = 2;
				track++;
			}
		}
	}
	else if (co == 1 && tat == 2) {
		*Rubik[0][0] = RotateZ(-Theta[Zaxis]) * *Rubik[0][0];
		*Rubik[0][1] = RotateZ(-Theta[Zaxis]) * *Rubik[0][1];
		*Rubik[0][2] = RotateZ(-Theta[Zaxis]) * *Rubik[0][2];
		*Rubik[0][3] = RotateZ(-Theta[Zaxis]) * *Rubik[0][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[3][1];
			Rubik[3][1] = Rubik[4][1];
			Rubik[4][1] = Rubik[2][0];
			Rubik[2][0] = Rubik[5][0];
			Rubik[5][0] = temp;
			temp = Rubik[3][0];
			Rubik[3][0] = Rubik[4][0];
			Rubik[4][0] = Rubik[2][1];
			Rubik[2][1] = Rubik[5][1];
			Rubik[5][1] = temp;
			temp = Rubik[0][2];
			Rubik[0][2] = Rubik[0][0];
			Rubik[0][0] = Rubik[0][1];
			Rubik[0][1] = Rubik[0][3];
			Rubik[0][3] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 1;
				track++;
			}
		}
	}
	else if (co == 2 && tat == 1) {
		*Rubik[2][0] = RotateY(Theta[Yaxis]) * *Rubik[2][0];
		*Rubik[2][1] = RotateY(Theta[Yaxis]) * *Rubik[2][1];
		*Rubik[2][2] = RotateY(Theta[Yaxis]) * *Rubik[2][2];
		*Rubik[2][3] = RotateY(Theta[Yaxis]) * *Rubik[2][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][2];
			Rubik[4][2] = Rubik[0][0];
			Rubik[0][0] = Rubik[5][0];
			Rubik[5][0] = Rubik[1][1];
			Rubik[1][1] = temp;
			temp = Rubik[4][0];
			Rubik[4][0] = Rubik[0][1];
			Rubik[0][1] = Rubik[5][2];
			Rubik[5][2] = Rubik[1][0];
			Rubik[1][0] = temp;
			temp = Rubik[2][2];
			Rubik[2][2] = Rubik[2][0];
			Rubik[2][0] = Rubik[2][1];
			Rubik[2][1] = Rubik[2][3];
			Rubik[2][3] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 2;
				track++;
			}
		}
	}
	else if (co == 2 && tat == 2) {
		*Rubik[2][0] = RotateY(-Theta[Yaxis]) * *Rubik[2][0];
		*Rubik[2][1] = RotateY(-Theta[Yaxis]) * *Rubik[2][1];
		*Rubik[2][2] = RotateY(-Theta[Yaxis]) * *Rubik[2][2];
		*Rubik[2][3] = RotateY(-Theta[Yaxis]) * *Rubik[2][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][2];
			Rubik[4][2] = Rubik[1][1];
			Rubik[1][1] = Rubik[5][0];
			Rubik[5][0] = Rubik[0][0];
			Rubik[0][0] = temp;
			temp = Rubik[4][0];
			Rubik[4][0] = Rubik[1][0];
			Rubik[1][0] = Rubik[5][2];
			Rubik[5][2] = Rubik[0][1];
			Rubik[0][1] = temp;
			temp = Rubik[2][2];
			Rubik[2][2] = Rubik[2][3];
			Rubik[2][3] = Rubik[2][1];
			Rubik[2][1] = Rubik[2][0];
			Rubik[2][0] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 1;
				track++;
			}
		}
	}
	else if (co == 3 && tat == 1) {
		*Rubik[5][0] = RotateX(Theta[Xaxis]) * *Rubik[5][0];
		*Rubik[5][1] = RotateX(Theta[Xaxis]) * *Rubik[5][1];
		*Rubik[5][2] = RotateX(Theta[Xaxis]) * *Rubik[5][2];
		*Rubik[5][3] = RotateX(Theta[Xaxis]) * *Rubik[5][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[2][3];
			Rubik[2][3] = Rubik[1][3];
			Rubik[1][3] = Rubik[3][1];
			Rubik[3][1] = Rubik[0][1];
			Rubik[0][1] = temp;
			temp = Rubik[2][1];
			Rubik[2][1] = Rubik[1][1];
			Rubik[1][1] = Rubik[3][3];
			Rubik[3][3] = Rubik[0][3];
			Rubik[0][3] = temp;
			temp = Rubik[5][2];
			Rubik[5][2] = Rubik[5][3];
			Rubik[5][3] = Rubik[5][1];
			Rubik[5][1] = Rubik[5][0];
			Rubik[5][0] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 2;
				track++;
			}
		}
	}
	else if (co == 3 && tat == 2) {
		*Rubik[5][0] = RotateX(-Theta[Xaxis]) * *Rubik[5][0];
		*Rubik[5][1] = RotateX(-Theta[Xaxis]) * *Rubik[5][1];
		*Rubik[5][2] = RotateX(-Theta[Xaxis]) * *Rubik[5][2];
		*Rubik[5][3] = RotateX(-Theta[Xaxis]) * *Rubik[5][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[2][3];
			Rubik[2][3] = Rubik[0][1];
			Rubik[0][1] = Rubik[3][1];
			Rubik[3][1] = Rubik[1][3];
			Rubik[1][3] = temp;
			temp = Rubik[2][1];
			Rubik[2][1] = Rubik[0][3];
			Rubik[0][3] = Rubik[3][3];
			Rubik[3][3] = Rubik[1][1];
			Rubik[1][1] = temp;
			temp = Rubik[5][2];
			Rubik[5][2] = Rubik[5][0];
			Rubik[5][0] = Rubik[5][1];
			Rubik[5][1] = Rubik[5][3];
			Rubik[5][3] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 1;
				track++;
			}
		}
	}
	if (co == 4 && tat == 1) {
		*Rubik[1][0] = RotateZ(Theta[Zaxis]) * *Rubik[1][0];
		*Rubik[1][1] = RotateZ(Theta[Zaxis]) * *Rubik[1][1];
		*Rubik[1][2] = RotateZ(Theta[Zaxis]) * *Rubik[1][2];
		*Rubik[1][3] = RotateZ(Theta[Zaxis]) * *Rubik[1][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][3];
			Rubik[4][3] = Rubik[3][3];
			Rubik[3][3] = Rubik[5][2];
			Rubik[5][2] = Rubik[2][2];
			Rubik[2][2] = temp;
			temp = Rubik[4][2];
			Rubik[4][2] = Rubik[3][2];
			Rubik[3][2] = Rubik[5][3];
			Rubik[5][3] = Rubik[2][3];
			Rubik[2][3] = temp;
			temp = Rubik[1][0];
			Rubik[1][0] = Rubik[1][2];
			Rubik[1][2] = Rubik[1][3];
			Rubik[1][3] = Rubik[1][1];
			Rubik[1][1] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 2;
				track++;
			}
		}
	}
	if (co == 4 && tat == 2) {
		*Rubik[1][0] = RotateZ(-Theta[Zaxis]) * *Rubik[1][0];
		*Rubik[1][1] = RotateZ(-Theta[Zaxis]) * *Rubik[1][1];
		*Rubik[1][2] = RotateZ(-Theta[Zaxis]) * *Rubik[1][2];
		*Rubik[1][3] = RotateZ(-Theta[Zaxis]) * *Rubik[1][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][3];
			Rubik[4][3] = Rubik[2][2];
			Rubik[2][2] = Rubik[5][2];
			Rubik[5][2] = Rubik[3][3];
			Rubik[3][3] = temp;
			temp = Rubik[4][2];
			Rubik[4][2] = Rubik[2][3];
			Rubik[2][3] = Rubik[5][3];
			Rubik[5][3] = Rubik[3][2];
			Rubik[3][2] = temp;
			temp = Rubik[1][0];
			Rubik[1][0] = Rubik[1][1];
			Rubik[1][1] = Rubik[1][3];
			Rubik[1][3] = Rubik[1][2];
			Rubik[1][2] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 1;
				track++;
			}
		}
	}
	else if (co == 5 && tat == 1) {
		*Rubik[3][0] = RotateY(Theta[Yaxis]) * *Rubik[3][0];
		*Rubik[3][1] = RotateY(Theta[Yaxis]) * *Rubik[3][1];
		*Rubik[3][2] = RotateY(Theta[Yaxis]) * *Rubik[3][2];
		*Rubik[3][3] = RotateY(Theta[Yaxis]) * *Rubik[3][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][3];
			Rubik[4][3] = Rubik[0][2];
			Rubik[0][2] = Rubik[5][1];
			Rubik[5][1] = Rubik[1][3];
			Rubik[1][3] = temp;
			temp = Rubik[4][1];
			Rubik[4][1] = Rubik[0][3];
			Rubik[0][3] = Rubik[5][3];
			Rubik[5][3] = Rubik[1][2];
			Rubik[1][2] = temp;
			temp = Rubik[3][2];
			Rubik[3][2] = Rubik[3][0];
			Rubik[3][0] = Rubik[3][1];
			Rubik[3][1] = Rubik[3][3];
			Rubik[3][3] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 2;
				track++;
			}
		}
	}
	else if (co == 5 && tat == 2) {
		*Rubik[3][0] = RotateY(-Theta[Yaxis]) * *Rubik[3][0];
		*Rubik[3][1] = RotateY(-Theta[Yaxis]) * *Rubik[3][1];
		*Rubik[3][2] = RotateY(-Theta[Yaxis]) * *Rubik[3][2];
		*Rubik[3][3] = RotateY(-Theta[Yaxis]) * *Rubik[3][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[4][3];
			Rubik[4][3] = Rubik[1][3];
			Rubik[1][3] = Rubik[5][1];
			Rubik[5][1] = Rubik[0][2];
			Rubik[0][2] = temp;
			temp = Rubik[4][1];
			Rubik[4][1] = Rubik[1][2];
			Rubik[1][2] = Rubik[5][3];
			Rubik[5][3] = Rubik[0][3];
			Rubik[0][3] = temp;
			temp = Rubik[3][2];
			Rubik[3][2] = Rubik[3][3];
			Rubik[3][3] = Rubik[3][1];
			Rubik[3][1] = Rubik[3][0];
			Rubik[3][0] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 1;
				track++;
			}
		}
	}
	else if (co == 6 && tat == 1) {
		*Rubik[4][0] = RotateX(Theta[Xaxis]) * *Rubik[4][0];
		*Rubik[4][1] = RotateX(Theta[Xaxis]) * *Rubik[4][1];
		*Rubik[4][2] = RotateX(Theta[Xaxis]) * *Rubik[4][2];
		*Rubik[4][3] = RotateX(Theta[Xaxis]) * *Rubik[4][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[2][2];
			Rubik[2][2] = Rubik[1][2];
			Rubik[1][2] = Rubik[3][0];
			Rubik[3][0] = Rubik[0][0];
			Rubik[0][0] = temp;
			temp = Rubik[2][0];
			Rubik[2][0] = Rubik[1][0];
			Rubik[1][0] = Rubik[3][2];
			Rubik[3][2] = Rubik[0][2];
			Rubik[0][2] = temp;
			temp = Rubik[4][2];
			Rubik[4][2] = Rubik[4][3];
			Rubik[4][3] = Rubik[4][1];
			Rubik[4][1] = Rubik[4][0];
			Rubik[4][0] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 2;
				track++;
			}
		}
	}
	else if (co == 6 && tat == 2) {
		*Rubik[4][0] = RotateX(-Theta[Xaxis]) * *Rubik[4][0];
		*Rubik[4][1] = RotateX(-Theta[Xaxis]) * *Rubik[4][1];
		*Rubik[4][2] = RotateX(-Theta[Xaxis]) * *Rubik[4][2];
		*Rubik[4][3] = RotateX(-Theta[Xaxis]) * *Rubik[4][3];
		counter++;
		if (counter == 900) {
			mat4 *temp = Rubik[2][2];
			Rubik[2][2] = Rubik[0][0];
			Rubik[0][0] = Rubik[3][0];
			Rubik[3][0] = Rubik[1][2];
			Rubik[1][2] = temp;
			temp = Rubik[2][0];
			Rubik[2][0] = Rubik[0][2];
			Rubik[0][2] = Rubik[3][2];
			Rubik[3][2] = Rubik[1][0];
			Rubik[1][0] = temp;
			temp = Rubik[4][2];
			Rubik[4][2] = Rubik[4][0];
			Rubik[4][0] = Rubik[4][1];
			Rubik[4][1] = Rubik[4][3];
			Rubik[4][3] = temp;
			tat = 0;
			counter = 0;
			if (lock != 2) {
				tracker[track][0] = co;
				tracker[track][1] = 1;
				track++;
			}
		}
	}
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' | key == 'q')
		exit(0);
	if (key == 'W' | key == 'w') {//Rotates camera around origin for the user
		Theta[0] = Theta[1] = Theta[2] = 2.0;
		projection = RotateX(Theta[Xaxis]) * projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}
	if (key == 'S' | key == 's') {
		Theta[0] = Theta[1] = Theta[2] = 2.0;
		projection = RotateX(-Theta[Xaxis]) * projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}
	if (key == 'A' | key == 'a') {
		Theta[0] = Theta[1] = Theta[2] = 2.0;
		projection = RotateY(Theta[Yaxis]) * projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}
	if (key == 'D' | key == 'd') {
		Theta[0] = Theta[1] = Theta[2] = 2.0;
		projection = RotateY(-Theta[Yaxis]) * projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}
	if (key == '1' && tat == 0 && co != 0 && lock==1) {//Rotates selected cube
		tat = 1;
	}
	if (key == '2' && tat == 0 && co != 0 && lock==1) {
		tat = 2;
	}
	if (key == 'R' | key == 'r') {
		random();
	}
	if (key == 'E' | key == 'e') {
		solve();
	}
	if (key == 'H' | key == 'h') {
		std::cout << "2x2 Rubik Cube Commands: " << std::endl;
		std::cout << "Left Mouse Button: Select the face to be rotated (Also print selected faces initial position)." << std::endl;
		std::cout << "1, 2: Rotate the selected face clockwise/counterclockwise." << std::endl;
		std::cout << "W, A, S, D: Rotate the camera around the cube." << std::endl;
		std::cout << "R: Randomize the cube." << std::endl;
		std::cout << "E: Solves the cube." << std::endl;
		std::cout << "Q: Exit program." << std::endl;
		std::cout << "H: Print commands." << std::endl;
		std::cout << std::endl;
	}

}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && lock) {//Picking
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glFlush();
		y = glutGet(GLUT_WINDOW_HEIGHT) - y;

		unsigned char pixel[4];
		glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
		if (pixel[0] == 255 && pixel[1] == 0 && pixel[2] == 0) { std::cout << "Front face" << std::endl; if(tat == 0)co = 1; }
		else if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 255) { std::cout << "Top face" << std::endl; if (tat == 0)co = 2; }
		else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 0) { std::cout << "Right face" << std::endl; if (tat == 0)co = 6; }
		else if (pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 0) { std::cout << "Bottom face" << std::endl; if (tat == 0)co = 5; }
		else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) { std::cout << "Left face" << std::endl; if (tat == 0)co = 3; }
		else if (pixel[0] == 255 && pixel[1] == 128 && pixel[2] == 0) { std::cout << "Back face" << std::endl; if (tat == 0)co = 4; }
		else std::cout << "None" << std::endl;
		std::cout << std::endl;

		glutPostRedisplay();
	}
}


//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Color Cube");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutDisplayFunc(display); // set display callback function
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);



	glutMainLoop();
	return 0;
}
