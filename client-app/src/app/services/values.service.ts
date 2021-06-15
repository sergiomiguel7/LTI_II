import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {apiUrl} from '../../environments/environment';


@Injectable({
  providedIn: 'root'
})
export class ValuesService {

  constructor(private http: HttpClient) { }

  getConcentradores(){
    const url = `${apiUrl}/concentradores`;
    const response$ = this.http.get(url);
    return response$;
  }

  getSensores(id: any){
    const url = `${apiUrl}/sensores/${id}`;
    const response$ = this.http.get(url);
    return response$;
  }

  changeLight(body:any){
    const url = `${apiUrl}/mqtt`;
    const response$ = this.http.post(url, body);
    return response$;
  }

 patchConcentrador(body:any, id: number){
    const url = `${apiUrl}/concentradores/${id}`;
    const response$ = this.http.patch(url, body);
    return response$;
  }
}
