﻿using System;
using System.Collections.Generic;

 
 
 

namespace GraphicResearchHuiZhao
{
    public class ToolVertexByCircle : ToolBaseTriMesh
    {


        public ToolVertexByCircle(double width, double height, TriMesh mesh)
            : base(width, height, mesh)
        {

        }





        public override void MouseMove(Vector2D mouseMovePos, EnumMouseButton button)
        {
            if (mesh == null)
                return;
            base.MouseMove(mouseMovePos, button);

            SelectByCircle();


            TriMeshUtil.GroupVertice(mesh);
            OnChanged(EventArgs.Empty);
        }

        

        protected virtual void SelectByCircle()
        {
            SelectVertexByCircle(true);
        }

        



        public override void Draw()
        {
 

            if (this.IsMouseDown)
                OpenGLManager.Instance.DrawSelectionInterface(Width, Height, this.MouseDownPos.x, this.MouseDownPos.y, this.MouseCurrPos.x, this.MouseCurrPos.y, OpenGLFlatShape.Circle); 

        }
       

    }
}
