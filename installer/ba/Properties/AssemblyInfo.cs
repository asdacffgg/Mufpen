using System.Reflection;
using System.Windows;
using Mufpen.Setup;
using WixToolset.Mba.Core;

[assembly: AssemblyTitle("MufpenBootstrapperUI")]
[assembly: AssemblyDescription("Mufpen setup bootstrapper application")]
[assembly: AssemblyProduct("Mufpen")]
[assembly: AssemblyVersion("1.0.0.0")]
[assembly: AssemblyFileVersion("1.0.0.0")]

[assembly: ThemeInfo(
    ResourceDictionaryLocation.None,
    ResourceDictionaryLocation.SourceAssembly)]

// Burn discovers the BA through this attribute: the mbanative host reads it to
// find the factory that creates our BootstrapperApplication subclass. WiX v3
// pointed this at the BA class itself; v4 requires the factory.
[assembly: BootstrapperApplicationFactory(typeof(MufpenBaFactory))]
