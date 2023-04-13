#include "Framework.h"
#include "Viewport.h"

Viewport::Viewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	Set(width, height, x, y, minDepth, maxDepth);
}

Viewport::~Viewport()
{
	
}

void Viewport::RSSetViewport()
{
	D3D::GetDC()->RSSetViewports(1, &viewport);
}

void Viewport::Set(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	viewport.TopLeftX = this->x = x;
	viewport.TopLeftY = this->y = y;
	viewport.Width = this->width = width;
	viewport.Height = this->height = height;
	viewport.MinDepth = this->minDepth = minDepth;
	viewport.MaxDepth = this->maxDepth = maxDepth;

	RSSetViewport();
}

Vector3 Viewport::Project(Vector3 & _source, Matrix & _world, Matrix & _view, Matrix & _projection)
{
	Vector3 position = _source;
	Vector3 result;
	Matrix wvp = _world * _view * _projection;
	D3DXVec3TransformCoord(&result, &position, &wvp);

	result.x = ((result.x + 1) * 0.5f) * width + x;
	result.y = ((-result.y + 1) * 0.5f) * height + y;
	result.z = (result.x * (maxDepth - minDepth)) + minDepth;

	return result;
}

Vector3 Viewport::UnProject(Vector3 & _source, Matrix & _world, Matrix & _view, Matrix & _projection)
{
	Vector3 position = _source;
	Vector3 result;
	result.x = ((position.x - x) / width) * 2.0f - 1.0f;
	result.y = (((position.y - y) / height) * 2.0f - 1.0f) * -1.0f;
	result.z = ((position.z - minDepth) / (maxDepth - minDepth));

	Matrix wvp = _world * _view * _projection;
	D3DXMatrixInverse(&wvp, NULL, &wvp);

	D3DXVec3TransformCoord(&result, &result, &wvp);

	return result;
}
void Viewport::GetMouseRay(Vector3 * _outPosition, Vector3 * _outDirection, Matrix * _world)
{
	Matrix W;
	if (_world != NULL)
		memcpy(&W, _world, sizeof(Matrix));
	else
		D3DXMatrixIdentity(&W);

	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* Vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();

	Vector3 n, f;
	mouse.z = 0.0f;
	n = Vp->UnProject(mouse, W, V, P);

	mouse.z = 0.9999f;
	f = Vp->UnProject(mouse, W, V, P);

	*_outPosition = n;
	*_outDirection = f - n;
}