import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import {apiUrl} from '../../environments/environment';


@Injectable({
  providedIn: 'root'
})
export class AdminService {

  constructor(private http: HttpClient) { }

  getUsers(){
    const url = `${apiUrl}/admin/users`;
    const response$ = this.http.get(url);
    return response$;
  }

  getConcentradores(){
    const url = `${apiUrl}/admin/concentradores`;
    const response$ = this.http.get(url);
    return response$;
  }
}
