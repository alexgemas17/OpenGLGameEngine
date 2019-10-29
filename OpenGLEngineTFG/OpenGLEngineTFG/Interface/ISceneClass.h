class ISceneClass
{
public:
	virtual ~ISceneClass() {}

	/** M�todo que tiene  **/
	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void DrawScene(double deltaTime) = 0;

	virtual void Destroy() = 0;
};