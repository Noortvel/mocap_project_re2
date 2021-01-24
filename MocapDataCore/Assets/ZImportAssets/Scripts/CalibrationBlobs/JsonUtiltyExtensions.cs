using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class JsonUtiltyExtensions
{
    public static string FromList(IList list)
    { 
        if (list.Count > 0)
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder();
            sb.Append("[");
            sb.Append(JsonUtility.ToJson(list[0]));
            for(int i = 1; i < list.Count; i++)
            {
                sb.Append(",");
                sb.Append(JsonUtility.ToJson(list[i]));
            }
            sb.Append("]");
            return sb.ToString();
        }
        return "";
    }  
}
