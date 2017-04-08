import pymel.core as pm
import mtoa.utils as utils
import mtoa.ui.ae.utils as aeUtils
from mtoa.ui.ae.shaderTemplate import ShaderAETemplate
 
class AEdustyTemplate(ShaderAETemplate):
 
    def setup(self):
        self.beginScrollLayout()

        self.beginLayout("General", collapse=False)
        self.addControl("color", label="Input", annotation="Expects the maya snow node")

        self.addControl("radius", label="Radius")

        self.addControl("spread", label="Spread")

        self.addControl("traceSet", label="Trace Set")

        self.addControl("samples", label="Samples")
 
        self.endLayout()
 
        # include/call base class/node attributes
        pm.mel.AEdependNodeTemplate(self.nodeName)
 
        # Add Section for the extra controls not displayed before
        self.addExtraControls()
        self.endScrollLayout()