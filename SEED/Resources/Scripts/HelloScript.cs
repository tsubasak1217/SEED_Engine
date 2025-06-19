
public class HelloScript : IScript {

    public override void OnStart() {
        System.Console.WriteLine("Hello from HelloScript!");
    }

    public override void Update() {
        System.Console.WriteLine("Hello from HelloScript!");
    }
}