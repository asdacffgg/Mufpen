using WixToolset.Mba.Core;

namespace Mufpen.Setup
{
    public class MufpenBaFactory : BaseBootstrapperApplicationFactory
    {
        protected override IBootstrapperApplication Create(IEngine engine, IBootstrapperCommand command)
        {
            return new MufpenBa(engine, command);
        }
    }
}
